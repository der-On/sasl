#include "propsserv.h"

#include <string.h>
#include "md5.h"
#include "libavcallbacks.h"


using namespace xa;

ClientProp::ClientProp()
{
    memset(&lastValue, 0, sizeof(lastValue));
}


ClientProp::ClientProp(int id, int type, const std::string &name, 
        Properties *properties, SaslPropRef ref):
       id(id), type(type), name(name), properties(properties), ref(ref)
{
    sendNext = true;
    memset(&lastValue, 0, sizeof(lastValue));
}

ClientProp::~ClientProp()
{
    if (lastValue.buf)
        free(lastValue.buf);
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
        case PROP_STRING:
            {
                std::string s = properties->getProps(ref);
                return (! lastValue.buf) || (strcmp(s.c_str(), lastValue.buf));
            }
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
        case PROP_STRING:
            {
                std::string s = properties->getProps(ref);
                int len = s.length();
                if ((! lastValue.buf) || (len + 1 > lastValue.maxBufSize)) {
                    lastValue.maxBufSize = len + 20;
                    if (lastValue.buf)
                        free(lastValue.buf);
                    lastValue.buf = (char*)malloc(lastValue.maxBufSize);
                }
                strcpy(lastValue.buf, s.c_str());
                buffer.addUint16(len);
                buffer.add((const unsigned char*)s.c_str(), len);
            }
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

void ClientProp::setString(const std::string &value)
{
    properties->setProp(ref, value);
}




PropsServer::PropsServer(Log &log, Properties &properties): 
        log(log), server(log), properties(properties)
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
        log.error("tcp server error\n");
        err = -1;
    }

    for (std::list<PropsClient>::iterator i = clients.begin(); 
            i != clients.end(); )
    {
        if ((*i).update()) {
            log.debug("closing client connection\n");
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
    clients.push_back(PropsClient(log, secret, properties));
    clients.back().start(sock);
}

bool PropsServer::isRunning()
{
    return server.isRunning();
}




PropsClient::PropsClient(Log &log, const std::string &secret, Properties &properties): 
    log(log), con(log), secret(secret), properties(properties)
{
}


PropsClient::~PropsClient()
{
}


void PropsClient::start(int sock)
{
    log.debug("starting connection\n");
    if (con.setSocket(sock)) {
        log.error("error witching client to non-blockng mode\n");
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
        log.debug("client closed\n");
        return -1;
    }
    int res = con.update();
    if (res) {
        log.error("error updaing client connection\n");
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
    if (4 > buffer.getFilled())
        return;

    if (memcmp(buffer.getData(), "NP2\n", 4)) {
        log.error("invalid protocol!\n");
        stop();
        return;
    }

    buffer.remove(4);

    con.send((unsigned char*)"NP1\n", 4);

    for (int i = 0; i < 16; i++)
        seed[i] = (unsigned char)rand();
    
    con.send(seed, 16);

    state = AUTH_VERIFY;
}


void PropsClient::doVerify(NetBuf &buffer)
{
    if (16 > buffer.getFilled())
        return;

    md5_state_t md5;
    md5_init(&md5);
    md5_append(&md5, (const md5_byte_t*)"NP2\n", 4);
    md5_append(&md5, seed, 16);
    md5_append(&md5, (const md5_byte_t*)secret.c_str(), secret.length());
    unsigned char digest[16];
    md5_finish(&md5, digest);

    bool passed = ! memcmp(digest, buffer.getData(), 16);
    buffer.remove(16);

    if (passed) {
        con.send((unsigned char*)"PASS", 4);
        state = COMMAND;
    } else {
        con.send((unsigned char*)"DENY", 4);
        con.sendAll();
        stop();
    }
}


void PropsClient::handleSubscription(NetBuf &buffer)
{
    if (4 > buffer.getFilled())
        return;
    unsigned int nameSize = buffer.getData()[3];
    if (nameSize + 6 > buffer.getFilled())
        return;

    int command = buffer.getData()[0];
    int type = buffer.getData()[1];
    int id = buffer.getData()[2];
    int maxSize = netToInt16(buffer.getData() + 4);
    std::string name((char*)buffer.getData() + 6, nameSize);
    buffer.remove(nameSize + 6);

    if ((1 > type) || (4 < type)) {
        log.error("Invalid property type %i\n", type);
        stop();
        return;
    }

    SaslPropRef prop;

    if (5 == command)
        prop = properties.createProp(name, type, maxSize);
    else
        prop = properties.getProp(name, type);

    if (! prop) {
        log.error("Can't reference property %s\n", name.c_str());
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
        log.error("Invalid property type %i\n", command[2]);
        stop();
        return;
    }
    sz += dataSz;

    if (buffer.getFilled() < sz)
        return;

    if (PROP_STRING == getPropTypeSize(command[2])) {
        dataSz = netToInt16(command + 5);
        sz += dataSz;
    }
    if (buffer.getFilled() < sz)
        return;

    lastSetSerial = netToInt16(command + 3);

    std::map<int, ClientProp>::iterator i = propRefs.find(command[1]);
    if (i == propRefs.end()) {
        log.warning("preoperty %i doesn't exists\n", command[1]);
        buffer.remove(sz);
        stop();
        return;
    }
    ClientProp &prop = (*i).second;

    switch (command[2]) {
        case PROP_INT: prop.setInt(netToInt32(command + 5)); break;
        case PROP_FLOAT: prop.setFloat(netToFloat(command + 5)); break;
        case PROP_DOUBLE: prop.setDouble(netToDouble(command + 5)); break;
        case PROP_STRING:
            prop.setString(std::string((const char*)command + 7, dataSz));
            break;
        default:
            log.error("invalid property type %i\n", command[2]);
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
                log.error("Invalid command %i\n", command);
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

