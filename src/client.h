#pragma once
#include "lib.h"

class Client : public AbstractInet{
    int _socket = -1;
public:
    void listen() override;
    bool send(const char* bytes, const size_t bufLen,
            const int socket, const sockaddr* dest) override;
    void sendMesg();
};