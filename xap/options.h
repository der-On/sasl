#ifndef __OPTIONS_H__
#define __OPTIONS_H__


#include <string>


namespace xap {


/// Options storage
class Options
{
    private:
        /// Path to config file
        std::string path;

        /// port number
        int port;

        /// Password for remote connections
        std::string secret;

        /// True if server auto start enabled
        bool autoStartServer;

    public:
        /// Default constructor
        Options() { };

        /// Create default config
        Options(const std::string &path);

    public:
        /// Load options
        void load();

        /// Returns server port number
        int getPort() const { return port; }

        /// Set new port number
        void setPort(int port) { this->port = port; }

        /// Returns server password
        const std::string& getSecret() const { return secret; }

        /// Set server password
        void setSecret(const std::string &secret) { this->secret = secret; }

        /// Returns true if server auto start enabled
        bool isAutoStartServer() const { return autoStartServer; }

        /// Enable or disable server auto start
        void enableAutoStartServer(bool enable) { autoStartServer = enable; }

        /// Save config file
        void save();
};


};


#endif

