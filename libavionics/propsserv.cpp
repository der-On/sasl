#include "propsserv.h"

#include <string.h>
#include "md5.h"
#include "xcallbacks.h"


using namespace xa;


ClientProp::ClientProp(int id, int type, const std::string &name, 
        Properties *properties, PropRef ref):
       id(id), type(type), name(name), properties(properties), ref(ref)
{
    sendNext = true;
}


bool ClientProp::isChanged()
{
    if (sendNext)
        return true;

    switch (type) {
        case PROP_INT:
            return properties->getPropi(ref) != lastValue.intValue;
        case PROP_FLOAT:
            return properties->getPropf(ref) != lastValue.floatValue;
        case PROP_DOUBLE:
            return properties->getPropd(ref) != lastValue.doubleValue;
        default:
            return false;
    }
}


void ClientProp::send(NetBuf &buffer)
{
    sendNext = false;

    buffer.addUint8(id);
    switch (type) {
        case PROP_INT: 
            lastValue.intValue = properties->getPropi(ref);
            buffer.addInt32(lastValue.intValue);
            break;
        case PROP_FLOAT:
            lastValue.floatValue = properties->getPropf(ref);
            buffer.addFloat(lastValue.floatValue);
            break;
        case PROP_DOUBLE:
            lastValue.doubleValue = properties->getPropf(ref);
            buffer.addDouble(lastValue.doubleValue);
            break;
    }
}


void ClientProp::setInt(int value)
{
    properties->setProp(ref, value);
}


void ClientProp::setFloat(float value)
{
    properties->setProp(ref, value);
}


void ClientProp::setDouble(double value)
{
    properties->setProp(ref, value);
}




PropsServer::PropsServer(Properties &properties): properties(properties)
{
    server.setCallback(this);
}


PropsServer::~PropsServer()
{
    server.stop();
}


int PropsServer::start(const char *password, int port)
{
    secret = password;
    return server.start(port);
}


int PropsServer::update()
{
    int err = 0;

    if (server.update()) {
printf("tcp server error\n");
        err = -1;
    }

    for (std::list<PropsClient>::iterator i = clients.begin(); 
            i != clients.end(); )
    {
        if ((*i).update()) {
printf("closing client connection\n");
            i = clients.erase(i);
        } else
            i++;
    }

    return err;
}


void PropsServer::stop()
{
    server.stop();
    clients.clear();
}


void PropsServer::onConnectionReceived(int sock)
{
    clients.push_back(PropsClient(secret, properties));
    clients.back().start(sock);
}

bool PropsServer::isRunning()
{
    return server.isRunning();
}




PropsClient::PropsClient(const std::string &secret, Properties &properties): 
    secret(secret), properties(properties)
{
}


PropsClient::~PropsClient()
{
}


void PropsClient::start(int sock)
{
printf("starting connection\n");
    if (con.setSocket(sock)) {
        printf("error witching client to non-blockng mode\n");
        stop();
        return;
    }
    con.setCallback(this);
    state = AUTH_HANDSHAKE;
    lastSetSerial = 0;
}


int PropsClient::update()
{
    if (CLOSED == state) {
printf("client closed\n");
        return -1;
    }
    int res = con.update();
    if (res) {
        printf("error updaing client connection\n");
        stop();
    }
    return res;
}


void PropsClient::onDataReceived(NetBuf &buffer)
{
    switch (state) {
        case AUTH_HANDSHAKE: doHandshake(buffer); break;
        case AUTH_VERIFY: doVerify(buffer); break;
        case COMMAND: doCommand(buffer); break;
        default: ;
    }
}


void PropsClient::doHandshake(NetBuf &buffer)
{
printf("checking handshake...\n");
    if (4 > buffer.getFilled())
        return;

printf("data received...\n");
    if (memcmp(buffer.getData(), "NP1\n", 4)) {
printf("invalid protocol!\n");
        stop();
        return;
    }

    buffer.remove(4);

printf("sending seed...\n");
    con.send((unsigned char*)"NP1\n", 4);

    for (int i = 0; i < 16; i++)
        seed[i] = (unsigned char)rand();
    
    con.send(seed, 16);

    state = AUTH_VERIFY;
}


void PropsClient::doVerify(NetBuf &buffer)
{
printf("verifying...\n");
    if (16 > buffer.getFilled())
        return;

printf("calculating md5...\n");
    md5_state_t md5;
    md5_init(&md5);
    md5_append(&md5, (const md5_byte_t*)"NP1\n", 4);
    md5_append(&md5, seed, 16);
    md5_append(&md5, (const md5_byte_t*)secret.c_str(), secret.length());
    unsigned char digest[16];
    md5_finish(&md5, digest);

    bool passed = ! memcmp(digest, buffer.getData(), 16);
    buffer.remove(16);

    if (passed) {
printf("sending PASS reply...\n");
        con.send((unsigned char*)"PASS", 4);
        state = COMMAND;
    } else {
printf("sending DENY reply...\n");
        con.send((unsigned char*)"DENY", 4);
        con.sendAll();
        stop();
    }
}


void PropsClient::handleSubscription(NetBuf &buffer)
{
    if (4 > buffer.getFilled())
        return;
    unsigned int size = buffer.getData()[3];
    if (size + 4 > buffer.getFilled())
        return;

    int command = buffer.getData()[0];
    int type = buffer.getData()[1];
    int id = buffer.getData()[2];
    std::string name((char*)buffer.getData() + 4, size);
    buffer.remove(size + 4);

    if ((1 > type) || (3 < type)) {
        printf("Invalid property type %i\n", type);
        stop();
        return;
    }

    PropRef prop;

    if (5 == command)
        prop = properties.createProp(name, type);
    else
        prop = properties.getProp(name, type);

    if (! prop) {
        printf("Can't reference property %s\n", name.c_str());
        return;
    }

    propRefs[id] = ClientProp(id, type, name, &properties, prop);
}


void PropsClient::handleGetProps(NetBuf &buffer)
{
    buffer.remove(1);

    std::list<ClientProp*> propsToSend;

    for (std::map<int, ClientProp>::iterator i = propRefs.begin();
            i != propRefs.end(); i++)
    {
        ClientProp &p = (*i).second;
        if (p.isChanged()) 
            propsToSend.push_back(&p);
    }
    
    con.getSendBuffer().addUint8(4);
    con.getSendBuffer().addUint8(propsToSend.size());
    con.getSendBuffer().addUint16(lastSetSerial);

    for (std::list<ClientProp*>::iterator i = propsToSend.begin(); 
            i != propsToSend.end(); i++)
    {
        ClientProp *p = *i;
        p->send(con.getSendBuffer());
    }
}


void PropsClient::handleSetProp(NetBuf &buffer)
{
    const unsigned char *command = buffer.getData();

    if (9 > buffer.getFilled())
        return;

    unsigned int sz = 5;
    int dataSz = getPropTypeSize(command[2]);
    if (! dataSz) {
        printf("Invalid property type %i\n", command[2]);
        stop();
        return;
    }
    sz += dataSz;

    if (buffer.getFilled() < sz)
        return;

    lastSetSerial = netToInt16(command + 3);

    std::map<int, ClientProp>::iterator i = propRefs.find(command[1]);
    if (i == propRefs.end()) {
        printf("preoperty %i doesn't exists\n", command[1]);
        buffer.remove(sz);
        stop();
        return;
    }
    ClientProp &prop = (*i).second;

    switch (command[2]) {
        case PROP_INT: prop.setInt(netToInt32(command + 5)); break;
        case PROP_FLOAT: prop.setFloat(netToFloat(command + 5)); break;
        case PROP_DOUBLE: prop.setDouble(netToDouble(command + 5)); break;
        default:
            printf("invalid property type %i\n", command[2]);
            stop();
    }
        
    buffer.remove(sz);
}


void PropsClient::doCommand(NetBuf &buffer)
{
    if (! buffer.getFilled())
        return;

    size_t lastFilled;
    do {
        lastFilled = buffer.getFilled();
        int command = buffer.getData()[0];
        switch (command) {
            case 1:
            case 5: handleSubscription(buffer);  break;
            case 2: handleSetProp(buffer);  break;
            case 3: handleGetProps(buffer);  break;
            default:
                printf("Invalid command %i\n", command);
                stop();
        }
    } while ((COMMAND == state) && (buffer.getFilled() != lastFilled) && 
            buffer.getFilled());
}


void PropsClient::stop()
{
    state = CLOSED;
    con.close();
}

