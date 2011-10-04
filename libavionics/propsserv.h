#ifndef __PROPS_SERV_H__
#define __PROPS_SERV_H__


#include <string>
#include <list>
#include <map>
#include "lownet.h"
#include "properties.h"
#include "log.h"


namespace xa {


/// Property requested by client
class ClientProp
{
    private:
        /// property ID at client side
        int id;

        /// Type of property
        int type;

        /// name of property
        std::string name;

        /// if true will resend it next time anyway
        bool sendNext;

        /// last known value of property
        union {
            int intValue;
            float floatValue;
            double doubleValue;

            char *buf;
            int maxBufSize;
        } lastValue;

        /// Properties subsystem
        Properties *properties;

        /// Reference to property
        SaslPropRef ref;

    public:
        ClientProp();

        /// Create new reference to property
        ClientProp(int id, int type, const std::string &name, 
                Properties *properties, SaslPropRef ref);
        
        ~ClientProp();

    public:
        /// Returns true if property needed to send
        bool isChanged();

        /// Write property to buffer
        void send(NetBuf &buffer);

        /// Set property value as integer
        void setInt(int value);
        
        /// Set property value as float
        void setFloat(float value);
        
        /// Set property value as double
        void setDouble(double value);
        
        /// Set property value as string
        void setString(const std::string &value);
};



/// Properties client connection
class PropsClient: private NetReceiver
{
    private:
        /// Logger to use
        Log log;

        /// Connection to client
        AsyncCon con;

        enum State {
            AUTH_HANDSHAKE,
            AUTH_VERIFY,
            COMMAND,
            CLOSED
        };

        /// Current state of client
        State state;

        /// secret word
        const std::string &secret;

        /// random sequence
        unsigned char seed[16];

        /// Properties subsystem
        Properties &properties;

        /// Properties names.
        std::map<int, ClientProp> propRefs;

        /// last seen set property serial
        int lastSetSerial;

    public:
        /// Create new connection to client
        PropsClient(Log &log, const std::string &secret, Properties &properties);

        /// Destroy connection to client
        ~PropsClient();

    public:
        /// move client to working state
        void start(int sock);

        /// proceed connection operations
        int update();

        /// shutdown connection
        void stop();

    private:
        /// called on data received
        virtual void onDataReceived(NetBuf &buffer);

        /// process handshake message
        void doHandshake(NetBuf &buffer);
        
        /// process authentication message
        void doVerify(NetBuf &buffer);
        
        /// process command message
        void doCommand(NetBuf &buffer);

        /// Handle subscription message
        void handleSubscription(NetBuf &buffer);
        
        /// Handle set property value message
        void handleSetProp(NetBuf &buffer);
        
        /// Handle get properties values message
        void handleGetProps(NetBuf &buffer);
};


/// Serve properties connections
class PropsServer: private ConnectionAcceptor
{
    private:
        /// logger to use
        Log &log;

        /// secret word
        std::string secret;

        /// TCP server object
        TcpServer server;

        /// Active connetions
        std::list<PropsClient> clients;
        
        /// Properties subsystem
        Properties &properties;

    public:
        /// create props server
        PropsServer(Log &log, Properties &properties);

        /// destroy props server
        virtual ~PropsServer();

    public:
        /// Start properties server
        int start(const char *secret, int port);

        /// Do networking communications
        int update();

        /// stop props server
        void stop();

        /// Returns true if server is running
        bool isRunning();

    private:
        /// create new connection
        virtual void onConnectionReceived(int sock);
};


};


#endif

