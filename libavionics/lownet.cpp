#include "lownet.h"

#include "exception.h"
#include "xcallbacks.h"

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#ifndef WINDOWS
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#endif
#include <errno.h>
#include <cstdio>


#ifdef WINDOWS
static void logf(char *str, ...)
{
    FILE *f = fopen("net.log", "a");
    va_list args;    
    va_start (args, str);
    if (! f) return;
    std::vfprintf(f, str, args);
    va_end(args);
    fclose(f);
}

#define printf logf
#endif

// MSG_NOSIGNAL does not exist on OS X and is never sent anyway
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

using namespace xa;


NetBuf::NetBuf() 
{
    allocated = 2048;
    data = (unsigned char*)malloc(allocated);
    filled = 0;
}

NetBuf::NetBuf(const NetBuf &nb)
{
    allocated = nb.allocated;
    filled = nb.filled;
    data = (unsigned char*)malloc(allocated);
    memcpy(data, nb.data, nb.filled);
}

NetBuf::~NetBuf() 
{
    if (data)
        free(data);
    data = NULL;
}


void NetBuf::ensureHasSpace(size_t size) 
{
    if (size + filled <= allocated)
        return;
    size_t newSize = ((filled + size) / 1024 + 1) * 1024;
    data = (unsigned char*)realloc(data, newSize);
    allocated = newSize;
}


void NetBuf::add(const unsigned char *s, size_t size) 
{
    ensureHasSpace(size);
    memcpy(data + filled, s, size);
    filled += size;
}


void NetBuf::addUint8(unsigned char v)
{
    add(&v, 1);
}


void NetBuf::addUint16(int v)
{
    uint16_t l = htons((uint16_t)v);
    add((const unsigned char *)&l, sizeof(l));
}



void NetBuf::addInt32(int v)
{
    uint32_t l = htonl((uint32_t)v);
    add((const unsigned char *)&l, sizeof(l));
}


void NetBuf::addFloat(float v)
{
    add((const unsigned char *)&v, sizeof(v));
}


void NetBuf::addDouble(double v)
{
    add((const unsigned char *)&v, sizeof(v));
}



void NetBuf::remove(size_t size) {
    if (size >= filled)
        filled = 0;
    else {
        memmove(data, data + size, filled - size);
        filled -= size;
    }
}


void NetBuf::increaseFilled(size_t size) {
    filled += size;
}


int xa::netToInt16(const unsigned char *data)
{
    return (int)ntohs(*(uint16_t*)data);
}


int xa::netToInt32(const unsigned char *data)
{
    return (int)ntohl(*(uint32_t*)data);
}


float xa::netToFloat(const unsigned char *data)
{
    return *((float*)data);
}


double xa::netToDouble(const unsigned char *data)
{
    return *((double*)data);
}

int xa::getPropTypeSize(int type)
{
    switch (type) {
        case PROP_INT: return 4;
        case PROP_FLOAT: return sizeof(float);
        case PROP_DOUBLE: return sizeof(double);
        default: return 0;
    }
}


/// put socket to non-blocking mode
static int makeNonBlock(int sock)
{
#ifdef WINDOWS
    unsigned long int nonblock = 1;
    ioctlsocket(sock, FIONBIO, &nonblock);
#else
    int flags = fcntl(sock, F_GETFL, 0);
    if (0 > flags)
        return -1;
    if (0 > fcntl(sock, F_SETFL, flags | O_NONBLOCK))
        return -1;
#endif
    return 0;
}


// close socket
static void closeSocket(int sock)
{
#ifdef WINDOWS
    shutdown(sock, SD_BOTH);
    closesocket(sock);
#else
    shutdown(sock, SHUT_RDWR);
    close(sock);
#endif
}


AsyncCon::AsyncCon()
{
    sock = 0;
    receiver = NULL;
}


AsyncCon::~AsyncCon()
{
    close();
}


int AsyncCon::setSocket(int socket)
{
    if (socket) {
        if (makeNonBlock(socket))
            return -1;
    }
    sock = socket;
    return 0;
}


void AsyncCon::send(const unsigned char *data, size_t size)
{
    sendBuffer.add(data, size);
}


/// returns true if socket can send data
static bool canSend(int sock)
{
    fd_set set;
    struct timeval tv;

    FD_ZERO(&set);
#ifdef WINDOWS
    FD_SET((unsigned)sock, &set);
#else
    FD_SET(sock, &set);
#endif

    memset(&tv, 0, sizeof(tv));

    int res = select(sock + 1, NULL, &set, NULL, &tv);
    return (1 == res);
}


/// returns true if socket can receive data
static bool canReceive(int sock)
{
    fd_set set;
    struct timeval tv;

    FD_ZERO(&set);
#ifdef WINDOWS
    FD_SET((unsigned)sock, &set);
#else
    FD_SET(sock, &set);
#endif

    memset(&tv, 0, sizeof(struct timeval));

    int res = select(sock + 1, &set, NULL, NULL, &tv);
    if (-1 == res) 
        perror("error receiving data");
    return (1 == res);
}


/// wait until socket can send data
static bool waitForCanSend(int sock)
{
    fd_set set;

    FD_ZERO(&set);
#ifdef WINDOWS
    FD_SET((unsigned)sock, &set);
#else
    FD_SET(sock, &set);
#endif

    int res = select(sock + 1, NULL, &set, NULL, NULL);
    if ((-1 == res) && (EINTR != errno))
        return -1;

    return 0;
}


/// wait until socket can receive data
static bool waitForCanReceive(int sock)
{
    fd_set set;

    FD_ZERO(&set);
#ifdef WINDOWS
    FD_SET((unsigned)sock, &set);
#else
    FD_SET(sock, &set);
#endif

    int res = select(sock + 1, &set, NULL, NULL, NULL);
    if ((0 >= res) && (EINTR != errno))
        return -1;

    return 0;
}



int AsyncCon::sendMore()
{
    if (! sendBuffer.getFilled())
        return 0;

#ifdef WINDOWS
    size_t sent = ::send(sock, (const char*)sendBuffer.getData(), 
            sendBuffer.getFilled(), 0);
#else
    size_t sent = ::send(sock, sendBuffer.getData(), sendBuffer.getFilled(),
            MSG_NOSIGNAL | MSG_DONTWAIT);
#endif
    if (0 >= (int)sent) {
        if (EAGAIN != errno)
            return -1;
    } else {
        sendBuffer.remove(sent);
    }

    return 0;
}


int AsyncCon::recvMore()
{
    recvBuffer.ensureHasSpace(2048);

#ifdef WINDOWS
    size_t received = ::recv(sock, (char*)recvBuffer.getFreeSpace(), 2048, 0);
#else
    size_t received = ::recv(sock, recvBuffer.getFreeSpace(), 2048,
            MSG_NOSIGNAL | MSG_DONTWAIT);
#endif
    if (0 >= (int)received) {
        if (EAGAIN != errno)
            return -1;
        else
            return 0;
    } else {
        recvBuffer.increaseFilled(received);
    }
            
    if (receiver && recvBuffer.getFilled())
        receiver->onDataReceived(recvBuffer);

    return 0;
}


int AsyncCon::update()
{
    if (sendBuffer.getFilled() && canSend(sock))
        if (sendMore()) {
printf("error sending data\n");
            return -1;
        }

    if (canReceive(sock))
        if (recvMore()) {
printf("error receiving data\n");
            return -1;
        }

    return 0;
}


int AsyncCon::sendAll()
{
    while (sendBuffer.getFilled()) {
        if (canSend(sock)) {
            if (sendMore())
                return -1;
        } else
            if (waitForCanSend(sock))
                return -1;
    }
    return 0;
}


int AsyncCon::recvData(size_t size)
{
    while (recvBuffer.getFilled() < size) {
        if (canReceive(sock)) {
            if (recvMore())
                return -1;
        } else
            if (waitForCanReceive(sock))
                return -1;
    }
    return 0;
}


void AsyncCon::setCallback(NetReceiver *callback)
{
    receiver = callback;
}


void AsyncCon::close()
{
    if (sock) {
printf("closing connection\n");
        closeSocket(sock);
        sock = 0;
    }
}




int xa::establishConnection(const char *host, int port)
{
    struct sockaddr_in addr;

    /* First try it as aaa.bbb.ccc.ddd. */
    addr.sin_addr.s_addr = inet_addr(host);
    if ((uint32_t)-1 == addr.sin_addr.s_addr) {
        struct hostent *he = gethostbyname(host);
        if (! he)
            return -1;
        else
            addr.sin_addr.s_addr = ((struct in_addr*)*he->h_addr_list)->s_addr;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (0 > sock)
        return -1;

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr))) {
        closeSocket(sock);
        return -1;
    }

    return sock;
}



TcpServer::TcpServer()
{
    sock = 0;
}


TcpServer::~TcpServer()
{
    if (sock)
        stop();
}


void TcpServer::setCallback(ConnectionAcceptor *callback)
{
    acceptor = callback;
}


static int createSocket(int port)
{
    struct sockaddr_in serv_addr;
    int one, sock;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons((u_short)port);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    makeNonBlock(sock);

    one = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(one));
    if (bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        closeSocket(sock);
        return -1;
    }

    if (listen(sock, 20) < 0) {
        closeSocket(sock);
        return -1;
    }

    return sock;
}


int TcpServer::start(int port)
{
printf("really starting\n");
    if (sock)
        stop();

    sock = createSocket(port);
    if (-1 == sock) {
        sock = 0;
        return -1;
    }

    return 0;
}


void TcpServer::stop()
{
    if (sock) {
        closeSocket(sock);
        sock = 0;
    }
}


int TcpServer::update()
{
    struct sockaddr_in clntAddr;
    memset(&clntAddr, 0, sizeof(clntAddr));
#ifdef WINDOWS
    int addrlen = sizeof(clntAddr);
#else
    socklen_t addrlen = sizeof(clntAddr);
#endif

    if (canReceive(sock)) {
        int clntSock = accept(sock, (struct sockaddr*)&clntAddr, &addrlen);
printf("accept %i\n", clntSock);
        if (-1 == clntSock)
            return (EAGAIN == errno) ? 0 : -1;

        if (acceptor)
            acceptor->onConnectionReceived(clntSock);
        else
            closeSocket(clntSock);
    }

    return 0;
}

bool TcpServer::isRunning()
{
    return sock;
}

