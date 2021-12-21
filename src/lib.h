#pragma once
// posix
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

// stl
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <limits.h>
#include <algorithm>
#include <iostream>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define INPUT_IP "127.0.0.1"
#define OUTPUT_IP "127.0.0.1"
#define MAX_BUFFER 8192

enum Device {
    DevServer, DevClient,
};

class AbstractInet{
protected:
    int _socketTcp = -1;
    int _socketUdp = -1;
    bool _tcpstop = false,
         _udpstop  = false;
    unsigned short _port = 0;
public:
    bool bind(const unsigned short port, const char type, Device device);
    virtual void listen() = 0;
    virtual bool send(const char* bytes, const size_t bufLen,
                      const int socket, const sockaddr* dest) = 0;
};