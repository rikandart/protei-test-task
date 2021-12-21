#include "server.h"

// accept sockets reading thread
void Server::accept(Server* srv){
    while(srv->_loopRun){
        int acc_sock = ::accept(srv->_socketTcp, nullptr, NULL);
        if(acc_sock != INVALID_SOCKET){
            srv->_accSocks.load()->push_back(acc_sock);
        }
    }
}

void Server::listen(){
#ifdef MULTI_THREAD_PROCESSING
    std::thread(listenTCP, this).detach();
    std::thread(listenUDP, this).join();
#else
    std::string protocol;
    if(_socketTcp != INVALID_SOCKET){
        _socket = _socketTcp;
        if(::listen(_socket, MAX_CLIENTS) == SOCKET_ERROR){
            std::cout << "Erros listen on port " << _port << std::endl;
            return;
        }
        std::thread(Server::accept, this).detach();
        protocol = "TCP";
    } else{
        _socket = _socketUdp;
        protocol = "UDP";
    }
    std::vector<int> acc_socks;
    acc_socks.reserve(MAX_CLIENTS);
    _accSocks.store(&acc_socks);
    int acc_sock = INVALID_SOCKET;
    for(; _loopRun;){
        char buf[MAX_BUFFER];
        ssize_t received = -1;
        std::shared_ptr<sockaddr_in> from(nullptr);
        socklen_t senderLen = sizeof(sockaddr_in);
        if(_socket == _socketTcp){
            acc_socks.reserve(MAX_CLIENTS);
            if(_accSocks.load()->size()){
                for(std::vector<int>::iterator acc_sock_it = _accSocks.load()->begin();
                    acc_sock_it != _accSocks.load()->end(); acc_sock_it++){
                    received = recv(*acc_sock_it, buf, MAX_BUFFER, 0);
                    if(received > 0){
                        acc_sock = *acc_sock_it;
                        break;
                    } 
                    else if(received < 0) _accSocks.load()->erase(acc_sock_it);
                }
            }
        } else {
            from.reset(new sockaddr_in);
            received = recvfrom(_socket, buf, MAX_BUFFER, 0, (sockaddr*)(from.get()), &senderLen);
        }
        if(received != -1 && received > 0){
            std::cout << protocol << " " << " received:\n" << buf << "\n" <<std::endl;
            std::string data(buf);
            std::string strBuf;
            _dataParser.parse(data, ' ', strBuf);
            std::cout << protocol << " " << " sending:\n" << strBuf << "\n" <<std::endl;
            send(strBuf.c_str(), strBuf.length()+1, _socket == _socketTcp ? acc_sock : _socket, (sockaddr*)(from.get()));
        }
    }
    close(_socketTcp);
    shutdown(_socketTcp, SHUT_RDWR);
    close(_socketUdp);
    shutdown(_socketUdp, SHUT_RDWR);
#endif
}

#ifdef MULTI_THREAD_PROCESSING
std::mutex Server::_parseMtx;
std::mutex Server::_logMtx;
std::mutex Server::_answerMtx;

void Server::listenTCP(Server* srv){
    srv->_listenTCP();
}

void Server::listenUDP(Server* srv){
    srv->_listenUDP();
}

void Server::_listenTCP(){
    _tcpstop = false;
    std::unique_lock<std::mutex> lck(_logMtx, std::defer_lock);
    lck.lock();
    if(::listen(_socketTcp, MAX_CLIENTS) == SOCKET_ERROR) return;
    lck.unlock();
    for(;!_tcpstop;){
        char buf[MAX_BUFFER];
        int acc_sock = ::accept(_socketTcp, nullptr, NULL);
        if(acc_sock == INVALID_SOCKET) continue;
        if(_atomClientsCount.load() == MAX_CLIENTS) continue;
        ssize_t received = recv(acc_sock, buf, MAX_BUFFER, 0);
        if(received != -1 && received > 0){
            lck.lock();
            std::cout << "TCP received:\n" << buf << "\n" <<std::endl;
            lck.unlock();
            std::string data(buf);
            std::thread(runParser, this, data, acc_sock, std::shared_ptr<sockaddr_in>(nullptr)).detach();
            _atomClientsCount.fetch_add(1);
        }
    }
    close(_socketTcp);
    shutdown(_socketTcp, SHUT_RDWR);
}

void Server::_listenUDP(){
    _udpstop = false;
    std::unique_lock<std::mutex> lck(_logMtx, std::defer_lock);
    for(;!_udpstop;){
        char buf[MAX_BUFFER];
        std::shared_ptr<sockaddr> from(new sockaddr);
        socklen_t senderLen = sizeof(sockaddr);
        if(_atomClientsCount.load() == MAX_CLIENTS) continue;
        ssize_t received = recvfrom(_socketTcp, buf, MAX_BUFFER, 0,
                                    from.get(), &senderLen);
        if(received != -1 && received > 0){
            lck.lock();
            std::cout << "UDP received:\n" << buf << "\n" << std::endl;
            lck.unlock();
            std::string data(buf);
            std::thread(runParser, this, data, _socketUdp, from).detach();
            _atomClientsCount.fetch_add(1);
        }
    }
    close(_socketUdp);
    shutdown(_socketUdp, SHUT_RDWR);
}

void Server::runParser(Server* srv, const std::string& data,
                        const int socket, std::shared_ptr<sockaddr_in> dest){
    std::unique_lock<std::mutex> lck(Server::_parseMtx);
    std::string strBuf;
    bool parseRes = srv->_dataParser.parse(data, strBuf);
    if(!parseRes){
        srv->_atomClientsCount.fetch_sub(1);
        lck.unlock();
        return;
    }    
    lck.unlock();
    srv->send(strBuf.c_str(), strBuf.length()+1, socket, dest.get());
    srv->_atomClientsCount.fetch_sub(1);
}
#endif

bool Server::send(const char* bytes, const size_t bufLen,
                const int socket, const sockaddr* dest){
#ifdef MULTI_THREAD_PROCESSING
    std::unique_lock<std::mutex> lck(_answerMtx);
#endif
    int sent = -1;
    if (socket == _socketUdp)
        sent = ::sendto(socket, bytes, bufLen, 0, dest, sizeof(sockaddr_in));
    else
        sent = ::send(socket, bytes, bufLen, 0);
        
#ifdef MULTI_THREAD_PROCESSING
    lck.unlock();
#endif
    
    return (size_t)sent == bufLen;
}

int main(int argc, char** argv){
    unsigned short port = 54321;
    char type = 0;
    for(int i = 0; i < argc; i++){
        if(!std::string(argv[i]).compare("-p")){
            try{
                int tmp = std::stoi(std::string(argv[++i]));
                if(tmp > (int)USHRT_MAX) throw std::invalid_argument("");
                port  = (unsigned short) tmp;
            } catch(const std::invalid_argument& e){
                std::cout << "Wrong port assignment" << std::endl;
            }
        }
        if(!std::string(argv[i]).compare("-u")){
            type = 'u';
        }
        if(!std::string(argv[i]).compare("-t")){
            type = 't';
        }
    }
    if(!type){
        std::cout << "Please specify protocol type: -t is for TCP, -u is for UDP" << std::endl;
        return -1;
    }
    std::cout << "Using port " << port << std::endl;
    Server srv;
    bool res = srv.bind(port, type, Device::DevServer);
    if(!res){
        std::cout << "Error binding port " << port << std::endl;
        return -1;
    }
    std::cout << std::endl;
    srv.listen();
    return 0;
}
