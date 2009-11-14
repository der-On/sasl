#include "xavionics.h"
#include "lownet.h"
#include "md5.h"
#include "lownet.h"
#include <string>
#include <vector>
#include <string.h>
#include <stdint.h>
#include <stdio.h>


using namespace xa;


struct NetProps;


/// Value of property
class PropValue
{
    private:
        /// Property ID
        int id;

        /// Property type
        int type;

        /// Name of property
        std::string name;

        /// Last known value of property
        union {
            int intValue;
            float floatValue;
            double doubleValue;
        } lastValue;

        /// Refernce to properties storage
        NetProps *props;

        /// Do not update till this revision
        int notUpdateTill;

    public:
        /// Create new property value
        PropValue(NetProps *props, int id, int type, const char *name);

    public:
        /// Returns type of property
        int getType() const { return type; }

        /// Returs property ID
        int getId() const { return id; }

        /// Returns name of property
        const std::string& getName() const { return name; }

        /// Returns reference to properties storage
        NetProps* getProps() { return props; }

        /// Returns property value as integer
        int getInt(int *err);

        /// Sets value of property.  returns non-zero on errors
        int setInt(int newValue);

        /// Returns property value as float
        float getFloat(int *err);

        /// Sets value of property.  returns non-zero on errors
        int setFloat(float newValue);

        /// Returns property value as double
        double getDouble(int *err);

        /// Sets value of property.  returns non-zero on errors
        int setDouble(double newValue);
        
        /// Load property value from raw data
        void parse(const unsigned char *data, int revision);

    private:
        /// Send set property value command to server
        int sendPropUpdate();
};


/// Storage of networked properties handles
struct NetProps
{
    AsyncCon con;
    std::vector<PropValue*> values;
    int propsToGo;
    uint16_t lastSetSerial;
    uint16_t curSetSerial;

    ~NetProps() {
        for (std::vector<PropValue*>::iterator i = values.begin();
                i != values.end(); i++)
            delete *i;
    }
};




PropValue::PropValue(NetProps *props, int id, int type, const char *name): 
    id(id), type(type), name(name), props(props)
{
    memset(&lastValue, 0, sizeof(lastValue));
    notUpdateTill = 0;
}


int PropValue::getInt(int *err)
{
    if (err)
        *err = 0;

    switch (type) {
        case PROP_INT: return lastValue.intValue;
        case PROP_FLOAT: return (int)lastValue.floatValue;
        case PROP_DOUBLE: return (int)lastValue.doubleValue;
    }

    if (err)
        *err = 1;
    return 0;
}


int PropValue::sendPropUpdate()
{
    props->lastSetSerial++;
    notUpdateTill = props->lastSetSerial;
    NetBuf &buf = props->con.getSendBuffer();
    buf.addUint8(2);
    buf.addUint8(id);
    buf.addUint8(type);
    buf.addUint16(props->lastSetSerial);
    switch (type) {
        case PROP_INT: buf.addInt32(lastValue.intValue);  return 0;
        case PROP_FLOAT: buf.addFloat(lastValue.floatValue);  return 0;
        case PROP_DOUBLE: buf.addDouble(lastValue.doubleValue);  return 0;
    }
    return -1;
}


int PropValue::setInt(int newValue)
{
    switch (type) {
        case PROP_INT: lastValue.intValue = newValue; break;
        case PROP_FLOAT: lastValue.floatValue = newValue;  break;
        case PROP_DOUBLE: lastValue.doubleValue = newValue; break;
    }
    return sendPropUpdate();
}


float PropValue::getFloat(int *err)
{
    if (err)
        *err = 0;

    switch (type) {
        case PROP_INT: 
            return lastValue.intValue;
        case PROP_FLOAT: 
            return lastValue.floatValue;
        case PROP_DOUBLE: 
            return (float)lastValue.doubleValue;
    }

    if (err)
        *err = 1;
    return 0;
}


int PropValue::setFloat(float newValue)
{
    switch (type) {
        case PROP_INT: lastValue.intValue = (int)newValue; break;
        case PROP_FLOAT: lastValue.floatValue = newValue;  break;
        case PROP_DOUBLE: lastValue.doubleValue = newValue; break;
    }
    return sendPropUpdate();
}


double PropValue::getDouble(int *err)
{
    if (err)
        *err = 0;

    switch (type) {
        case PROP_INT: return lastValue.intValue;
        case PROP_FLOAT: return lastValue.floatValue;
        case PROP_DOUBLE: return lastValue.doubleValue;
    }

    if (err)
        *err = 1;
    return 0;
}


int PropValue::setDouble(double newValue)
{
    switch (type) {
        case PROP_INT: lastValue.intValue = (int)newValue; break;
        case PROP_FLOAT: lastValue.floatValue = (float)newValue;  break;
        case PROP_DOUBLE: lastValue.doubleValue = newValue; break;
    }
    return sendPropUpdate();
}

        
void PropValue::parse(const unsigned char *data, int revision)
{
    if (! ((revision >= notUpdateTill) || 
            ((65530 < notUpdateTill) && (10 > revision))))
    {
        return;
    }

    notUpdateTill = revision;
    switch (type) {
        case PROP_INT: 
            lastValue.intValue = netToInt32(data); 
            break;
        case PROP_FLOAT: 
            lastValue.floatValue = netToFloat(data); 
            break;
        case PROP_DOUBLE: 
            lastValue.doubleValue = netToDouble(data); 
            break;
    }
}



/// Returns reference to property
static PropRef createPropRef(Props props, const char *name, int type, int cmd)
{
    NetProps *p = (NetProps*)props;
    if (! p)
        return NULL;

    int id = p->values.size() + 1;
    if ((255 < id) || (PROP_INT > type) || (PROP_DOUBLE < type)) {
        printf("invalid property type %i\n", type);
        return NULL;
    }

    for (int i = 0; i < id - 1; i++) {
        PropValue *v = p->values[i];
        if ((v->getType() == type) && (v->getName() == name))
            return v;
    }

    p->values.push_back(new PropValue(p, id, type, name));
    int len = strlen(name);

    NetBuf &buf = p->con.getSendBuffer();
    buf.addUint8(cmd);
    buf.addUint8(type);
    buf.addUint8(id);
    buf.addUint8(len);
    buf.add((unsigned char*)name, len);

    return p->values[id - 1];
}


/// Get reference to property
static PropRef getPropRef(Props props, const char *name, int type)
{
    return createPropRef(props, name, type, 1);
}

/// Get reference to property or create new property
static PropRef createProp(Props props, const char *name, int type)
{
    return createPropRef(props, name, type, 5);
}

/// create functional propert
static PropRef createFuncProp(Props props, const char *name, 
            int type, xa_prop_getter_callback getter, 
            xa_prop_setter_callback setter, 
            void *ref)
{
    printf("networked functional properties doesn't supported yet!\n");
    return NULL;
}

/// does nothing for now.  properties referenced forever
static void freePropRef(PropRef prop)
{
}


/// Returns property value as integer
static int getPropInt(PropRef prop, int *err)
{
    PropValue *value = (PropValue*)prop;
    if (! value) {
printf("invalid prop ref!\n");
        if (err)
            *err = 1;
        return 0;
    }

    return value->getInt(err);
}


/// Sets value of property as integer
/// Returns zero on cuccess or non-zero on error
static int setPropInt(PropRef prop, int newValue)
{
    PropValue *value = (PropValue*)prop;
    if (! value)
        return -1;
    
    return value->setInt(newValue);
}


/// Returns property value as float
static float getPropFloat(PropRef prop, int *err)
{
    PropValue *value = (PropValue*)prop;
    if (! value) {
printf("invalid prop ref!\n");
        if (err)
            *err = 1;
        return 0;
    }

    return value->getFloat(err);
}


/// Sets value of property as float
/// Returns zero on cuccess or non-zero on error
static int setPropFloat(PropRef prop, float newValue)
{
    PropValue *value = (PropValue*)prop;
    if (! value)
        return -1;
    
    return value->setFloat(newValue);
}


/// Returns property value as double
static double getPropDouble(PropRef prop, int *err)
{
    PropValue *value = (PropValue*)prop;
    if (! value) {
        if (err)
            *err = 1;
        return 0;
    }

    return value->getDouble(err);
}


/// Sets value of property as double
/// Returns zero on cuccess or non-zero on error
static int setPropDouble(PropRef prop, double newValue)
{
    PropValue *value = (PropValue*)prop;
    if (! value)
        return -1;
    
    return value->setDouble(newValue);
}


/// destroy properties
static void doneProps(Props props)
{
    NetProps *p = (NetProps*)props;
    if (p)
        delete p;
}


// do networked job
static int updateProps(Props props)
{
    NetProps *p = (NetProps*)props;
    if (! p)
        return -1;

    if (p->con.update())
        return -1;

    bool isPropsAvailable = p->propsToGo;

    NetBuf &buf = p->con.getRecvBuffer();
    if ((! p->propsToGo) && (2 <= buf.getFilled())) {
        int id = buf.getData()[0];
        p->propsToGo = buf.getData()[1];
        p->curSetSerial = netToInt16(buf.getData() + 2);
        buf.remove(4);
        if (4 != id) {
            printf("Invalid command %i\n", id);
            p->con.close();
            return -1;
        }
        isPropsAvailable = true;
    }

    while (p->propsToGo && (1 < buf.getFilled())) {
        int propId = buf.getData()[0];
        if ((! propId) || (propId > (int)p->values.size())) {
            printf("invalid property id %i\n", propId);
            p->con.close();
            return -1;
        }
        PropValue *v = p->values[propId - 1];
        int sz = getPropTypeSize(v->getType());
        if (buf.getFilled() < (unsigned)sz + 1)
            break;
        v->parse(buf.getData() + 1, p->curSetSerial);
        buf.remove(1 + sz);
        p->propsToGo--;
    }

    if ((! p->propsToGo) && (isPropsAvailable))
        p->con.getSendBuffer().addUint8(3);

    return 0;
}


static PropsCallbacks callbacks = { getPropRef, freePropRef, createProp, 
        createFuncProp, getPropInt, setPropInt, getPropFloat, 
        setPropFloat, getPropDouble, setPropDouble, updateProps, doneProps };


int xa_connect_to_server(XA xa, const char *host, int port, 
        const char *secret)
{
    int sock = establishConnection(host, port);
printf("connecting...\n");
    if (1 > sock)
        return -1;

    NetProps *np = new NetProps;
    np->con.setSocket(sock);
    AsyncCon &con = np->con;

printf("sending handshake...\n");
    con.send((unsigned char*)"NP1\n", 4);
    if (con.sendAll()) {
        delete np;
        return -1;
    }

printf("receiving reply...\n");
    if (con.recvData(20)) {
        delete np;
        return -1;
    }

    NetBuf &buf = con.getRecvBuffer();
    if (20 != buf.getFilled()) {
        delete np;
        return -1;
    }

printf("checking server reply...\n");
    md5_state_t md5;
    md5_init(&md5);
    md5_append(&md5, buf.getData(), 20);
    md5_append(&md5, (md5_byte_t*)secret, strlen(secret));
    md5_byte_t digest[16];
    md5_finish(&md5, digest);
    buf.remove(20);
    
    con.send(digest, 16);
    if (con.sendAll()) {
        delete np;
        return -1;
    }
    
printf("receiving result...\n");
    if (con.recvData(4) || (4 != buf.getFilled())) {
printf("can't receive result\n");
        delete np;
        return -1;
    }

    if (memcmp(buf.getData(), "PASS", 4)) {
printf("we are not allowed\n");
        delete np;
        return -1;
    }
    buf.remove(4);
printf("logged in!\n");

    np->propsToGo = 0;
    np->lastSetSerial = 0;

    xa_set_props(xa, &callbacks, np);
        
    np->con.getSendBuffer().addUint8(3);

    return 0;
}



