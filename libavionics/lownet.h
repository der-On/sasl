#ifndef __LOW_NET_H__
#define __LOW_NET_H__

// low-level network routinues


#include <stdlib.h>

#ifdef _MSC_VER
#define uint16_t unsigned __int16
#define uint32_t unsigned __int16
#endif


namespace xa {

/// Buffer for data
class NetBuf
{
    private:
        /// Pointer to data buffer
        unsigned char *data;

        /// size of allocated buffer
        size_t allocated;

        /// How much data located in buffer
        size_t filled;

    public:
        /// Create new empty buffer
        NetBuf();
        
        /// Copy buffer
        NetBuf(const NetBuf &netBuf);

        /// clean-up
        ~NetBuf();

    public:
        /// Make sure buffer size is at least size bytes.
        /// Allocates more memory if needed
        void ensureHasSpace(size_t size);

        /// append data to buffer
        void add(const unsigned char *s, size_t size);

        /// Append 1 byte to buffer.
        void addUint8(unsigned char v);
        
        /// Append 2 bytes to buffer.
        void addUint16(int v);

        /// Append 4 bytes to buffer.
        void addInt32(int v);

        /// Append float value to buffer.
        void addFloat(float v);
        
        /// Append double value to buffer.
        void addDouble(double v);

        /// Remove data from start of buffer
        void remove(size_t size);

        /// Returns pointer to data buffer
        unsigned char* getData() { return data; };

        /// Returns how much bytes stored in buffer
        size_t getFilled() { return filled; };
        
        /// Returns pointer to free space at data buffer
        unsigned char* getFreeSpace() { return data + filled; };

        /// Mark more space as filled
        void increaseFilled(size_t size);
};


/// Convert data from network to short integer
int netToInt16(const unsigned char *data);

/// Convert data from network to integer
int netToInt32(const unsigned char *data);

/// Convert data from network to float
float netToFloat(const unsigned char *data);

/// Convert data from network to double
double netToDouble(const unsigned char *data);

/// Returns size of marshaled properties
int getPropTypeSize(int type);


/// Receiver of network data
class NetReceiver
{
    public:
        virtual ~NetReceiver() { };

        /// called when data was received.
        /// callback is responsible for removing data from buffer
        virtual void onDataReceived(NetBuf &buffer) = 0;
};


/// Low-level async net routinues
class AsyncCon
{
    private:
        /// Socket handle
        int sock;

        /// Buffer stored data needed to send
        NetBuf sendBuffer;
        
        /// Place for received data
        NetBuf recvBuffer;

        /// Data receiver callback
        NetReceiver *receiver;

    public:
        /// Create async net struture
        AsyncCon();

        /// Destroy async net structure
        ~AsyncCon();

    public:
        /// Set socket.
        /// Turns socket to non-blocking mode
        int setSocket(int sock);

        /// Schedule data to send
        void send(const unsigned char *data, size_t size);

        /// Process async event
        int update();

        /// send all data from buffer
        int sendAll();

        /// Block until at least size bytes of data will be received.
        int recvData(size_t size);

        /// Returns buffer with received data.
        NetBuf& getRecvBuffer() { return recvBuffer; };
        
        /// Returns buffer with data to send.
        NetBuf& getSendBuffer() { return sendBuffer; };

        /// Set data receiver callback
        void setCallback(NetReceiver *receiver);

        /// Close connection
        void close();

    private:
        /// Send next portion of data
        int sendMore();
        
        /// Receive next portion of data
        int recvMore();

};


/// Connect to remote server
int establishConnection(const char *host, int port);


class ConnectionAcceptor
{
    public:
        virtual ~ConnectionAcceptor() { };

        /// Called on new connection established
        virtual void onConnectionReceived(int sock) = 0;
};


/// Server-side socket
class TcpServer
{
    private:
        /// socket descriptor
        int sock;

        /// Connection acceptor callback
        ConnectionAcceptor *acceptor;

    public:
        /// create server object
        TcpServer();
        
        /// destroy socket
        ~TcpServer();

    public:
        /// Set acceptor callback
        void setCallback(ConnectionAcceptor *acceptor);

        /// open server socket
        int start(int port);

        /// stop accepting connections
        void stop();

        /// process incoming connections
        int update();

        /// returns true if server is running
        bool isRunning();
};

};

#endif

