#include "propsclient.h"
#include "libavionics.h"
#include <string>
#include <vector>
#include <string.h>
#ifndef WINDOWS
#include <stdint.h>
#endif
#include <stdio.h>
#include "lownet.h"
#include "md5.h"
#include "utils.h"


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

            char *buf;
            int maxBufSize;
        } lastValue;

        /// Refernce to properties storage
        NetProps *props;

        /// Do not update till this revision
        int notUpdateTill;

    public:
        /// Create new property value
        PropValue(NetProps *props, int id, int type, const char *name);
        
        ~PropValue();

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
        
        /// Returns property value as double
        int getString(char *buf, int maxSize, int *err);

        /// Sets value of property.  returns non-zero on errors
        int setString(const char *newValue);
        
        /// Load property value from raw data
        void parse(const unsigned char *data, int revision);

    private:
        /// Send set property value command to server
        int sendPropUpdate();
};


/// Storage of networked properties handles
struct NetProps
{
    Log &log;
    AsyncCon con;
    std::vector<PropValue*> values;
    int propsToGo;
    uint16_t lastSetSerial;
    uint16_t curSetSerial;

    NetProps(Log &log): log(log), con(log) { };

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


PropValue::~PropValue()
{
    if (lastValue.buf)
        free(lastValue.buf);
}



int PropValue::getInt(int *err)
{
    if (err)
        *err = 0;

    switch (type) {
        case PROP_INT: return lastValue.intValue;
        case PROP_FLOAT: return (int)lastValue.floatValue;
        case PROP_DOUBLE: return (int)lastValue.doubleValue;
        case PROP_STRING: return strToInt(lastValue.buf);
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
        case PROP_STRING: 
            int len = 0;
            if (lastValue.buf)
                len = strlen(lastValue.buf);
            buf.addUint16(len);
            if (len)
                buf.add((unsigned char*)lastValue.buf, len);
            return 0;
    }
    return -1;
}


int PropValue::setInt(int newValue)
{
    switch (type) {
        case PROP_INT: lastValue.intValue = newValue; break;
        case PROP_FLOAT: lastValue.floatValue = newValue;  break;
        case PROP_DOUBLE: lastValue.doubleValue = newValue; break;
        case PROP_STRING: return setString(toString(newValue).c_str());
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
        case PROP_STRING: return strToFloat(lastValue.buf);
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
        case PROP_STRING: return setString(toString(newValue).c_str());
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
        case PROP_STRING: return strToDouble(lastValue.buf);
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
        case PROP_STRING: return setString(toString(newValue).c_str());
    }
    return sendPropUpdate();
}


int PropValue::getString(char *buf, int maxSize, int *err)
{
    if (err)
        *err = 0;

    if (PROP_STRING != type) {
        std::string s;
        switch (type) {
            case PROP_INT: s = toString(lastValue.intValue);  break;
            case PROP_FLOAT: s = toString(lastValue.floatValue);  break;
            case PROP_DOUBLE: s = toString(lastValue.doubleValue); break;
            default:
                if (err)
                    *err = 1;
                return 0;
        }
        int len = s.length();
        if ((len + 1 > maxSize) || (! buf)) {
            if (err)
                *err = 1;
        } else
            strcpy(buf, s.c_str());
        return len;
    } else {
        if (! lastValue.buf) {
            if (buf && (0 < maxSize))
                strcpy(buf, "");
            return 0;
        } else {
            int len = strlen(lastValue.buf);
            if (maxSize < len + 1) {
                if (err)
                    *err = 1;
            } else
                strcpy(buf, lastValue.buf);
            return len;
        }
    }
}


int PropValue::setString(const char *newValue)
{
    if (! newValue)
        newValue = "";

    switch (type) {
        case PROP_INT: lastValue.intValue = strToInt(newValue); break;
        case PROP_FLOAT: lastValue.floatValue = strToFloat(newValue);  break;
        case PROP_DOUBLE: lastValue.doubleValue = strToDouble(newValue); break;
        case PROP_STRING: 
            int len = strlen(newValue);
            if ((! lastValue.buf) || (len + 1 > lastValue.maxBufSize)) {
                lastValue.maxBufSize = len + 20;
                if (lastValue.buf)
                    free(lastValue.buf);
                lastValue.buf = (char*)malloc(lastValue.maxBufSize);
            }
            strcpy(lastValue.buf, newValue);
            break;
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
        case PROP_STRING: 
            int len = netToInt16(data);
            if ((! lastValue.buf) || (len + 1 > lastValue.maxBufSize)) {
                lastValue.maxBufSize = len + 20;
                if (lastValue.buf)
                    free(lastValue.buf);
                lastValue.buf = (char*)malloc(lastValue.maxBufSize);
            }
            memcpy(lastValue.buf, data + 2, len);
            lastValue.buf[len] = 0;
            break;
    }
}



/// Returns reference to property
static SaslPropRef createSaslPropRef(SaslProps props, const char *name, int type, 
        int maxSize, int cmd)
{
    NetProps *p = (NetProps*)props;
    if (! p)
        return NULL;

    int id = p->values.size() + 1;
    if ((255 < id) || (PROP_INT > type) || (PROP_STRING < type)) {
        p->log.error("invalid property type %i\n", type);
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
    buf.addUint8(maxSize);
    buf.add((unsigned char*)name, len);

    return p->values[id - 1];
}


/// Get reference to property
static SaslPropRef getSaslPropRef(SaslProps props, const char *name, int type)
{
    return createSaslPropRef(props, name, type, 1, 0);
}

/// Get reference to property or create new property
static SaslPropRef createProp(SaslProps props, const char *name, int type, int maxSize)
{
    return createSaslPropRef(props, name, type, 5, maxSize);
}

/// create functional propert
static SaslPropRef createFuncProp(SaslProps props, const char *name, 
            int type, int maxSize, sasl_prop_getter_callback getter, 
            sasl_prop_setter_callback setter, 
            void *ref)
{
    NetProps *p = (NetProps*)props;
    if (! p)
        return NULL;
    p->log.error("networked functional properties doesn't supported yet!\n");
    return NULL;
}

/// does nothing for now.  properties referenced forever
static void freeSaslPropRef(SaslPropRef prop)
{
}


/// Returns property value as integer
static int getPropInt(SaslPropRef prop, int *err)
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
static int setPropInt(SaslPropRef prop, int newValue)
{
    PropValue *value = (PropValue*)prop;
    if (! value)
        return -1;
    
    return value->setInt(newValue);
}


/// Returns property value as float
static float getPropFloat(SaslPropRef prop, int *err)
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
static int setPropFloat(SaslPropRef prop, float newValue)
{
    PropValue *value = (PropValue*)prop;
    if (! value)
        return -1;
    
    return value->setFloat(newValue);
}


/// Returns property value as double
static double getPropDouble(SaslPropRef prop, int *err)
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
static int setPropDouble(SaslPropRef prop, double newValue)
{
    PropValue *value = (PropValue*)prop;
    if (! value)
        return -1;
    
    return value->setDouble(newValue);
}


/// Returns property value as string
static int getPropString(SaslPropRef prop, char *buf, int maxSize, int *err)
{
    PropValue *value = (PropValue*)prop;
    if (! value) {
        if (err)
            *err = 1;
        return 0;
    }

    return value->getString(buf, maxSize, err);
}


/// Sets value of property as string
/// Returns zero on cuccess or non-zero on error
static int setPropString(SaslPropRef prop, const char *newValue)
{
    PropValue *value = (PropValue*)prop;
    if (! value)
        return -1;
    
    return value->setString(newValue);
}


/// destroy properties
static void doneProps(SaslProps props)
{
    NetProps *p = (NetProps*)props;
    if (p)
        delete p;
}


// do networked job
static int updateProps(SaslProps props)
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
            p->log.error("Invalid command %i\n", id);
            p->con.close();
            return -1;
        }
        isPropsAvailable = true;
    }

    while (p->propsToGo && (1 < buf.getFilled())) {
        int propId = buf.getData()[0];
        if ((! propId) || (propId > (int)p->values.size())) {
            p->log.error("invalid property id %i\n", propId);
            p->con.close();
            return -1;
        }
        PropValue *v = p->values[propId - 1];
        int sz = getPropTypeSize(v->getType());
        if (buf.getFilled() < (unsigned)sz + 1)
            break;
        if (PROP_STRING == v->getType())
            sz += netToInt16(buf.getData() + 1);
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


static SaslPropsCallbacks callbacks = { getSaslPropRef, freeSaslPropRef, createProp, 
        createFuncProp, getPropInt, setPropInt, getPropFloat, 
        setPropFloat, getPropDouble, setPropDouble, 
        getPropString, setPropString,
        updateProps, doneProps };


int xa::connectToServer(SASL sasl, Log &log, const char *host, int port, 
        const char *secret)
{
    int sock = establishConnection(host, port);
    log.debug("connecting...");
    if (1 > sock)
        return -1;

    NetProps *np = new NetProps(log);
    np->con.setSocket(sock);
    AsyncCon &con = np->con;

    con.send((unsigned char*)"NP2\n", 4);
    if (con.sendAll()) {
        delete np;
        return -1;
    }

    if (con.recvData(20)) {
        delete np;
        return -1;
    }

    NetBuf &buf = con.getRecvBuffer();
    if (20 != buf.getFilled()) {
        delete np;
        return -1;
    }

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
    
    if (con.recvData(4) || (4 != buf.getFilled())) {
        log.error("can't receive result");
        delete np;
        return -1;
    }

    if (memcmp(buf.getData(), "PASS", 4)) {
        log.error("we are not allowed");
        delete np;
        return -1;
    }
    buf.remove(4);
    log.debug("logged in!");

    np->propsToGo = 0;
    np->lastSetSerial = 0;

    sasl_set_props(sasl, &callbacks, np);
        
    np->con.getSendBuffer().addUint8(3);

    return 0;
}



