#pragma once
// local
#include "lib.h"
#include "parser.h"

#define MAX_CLIENTS 200
class Server : public AbstractInet{
    Parser _dataParser;
    int _socket = -1;
    std::atomic<std::vector<int>*> _accSocks;
#ifdef MULTI_THREAD_PROCESSING
    std::atomic<unsigned> _atomClientsCount;
    static std::mutex _parseMtx, _logMtx, _answerMtx;
    void _listenTCP();
    void _listenUDP();
#endif
    bool _loopRun = true;
public:
    Server(){
#ifdef MULTI_THREAD_PROCESSING
        _atomClientsCount.store(0);
#endif
    }
    void listen() override;
    bool send(const char* bytes, const size_t bufLen,
            const int socket, const sockaddr* dest) override;
    static void accept(Server* srv);
#ifdef MULTI_THREAD_PROCESSING
    static void runParser(Server* srv, const std::string& data,
                   const int socket, std::shared_ptr<sockaddr_in> dest);
    static void listenTCP(Server* srv);
    static void listenUDP(Server* srv);
#endif
};
