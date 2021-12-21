#include "lib.h"

bool AbstractInet::bind(const unsigned short port, const char type, Device device){
    int protocol = 0, packetType = 0;
    int* sock = nullptr;
    switch(type){
        case 't':
            protocol = IPPROTO_TCP;
            sock = &_socketTcp;
            packetType = SOCK_STREAM;
            break;
        case 'u':
            protocol = IPPROTO_UDP;
            sock = &_socketUdp;
            packetType = SOCK_DGRAM;
            break;
        default:
            return false;
    }
    
    *sock = socket(AF_INET, packetType, protocol);
    _port = port;
    if(*sock == INVALID_SOCKET) return false;
    struct sockaddr_in si;
    si.sin_family = AF_INET;
    si.sin_port = htons(port);
    int res = -1;
    switch(device){
        case Device::DevServer:
            si.sin_addr.s_addr = inet_addr(INPUT_IP);
            res = ::bind(*sock, (struct sockaddr*)&si, sizeof(si));
            break;
        case Device::DevClient:
            si.sin_addr.s_addr = inet_addr(OUTPUT_IP);
            res = ::connect(*sock, (struct sockaddr*)&si, sizeof(si));
            break;
        default:
            return false;
    }
    return res != SOCKET_ERROR;
}