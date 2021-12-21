#include "client.h"

void Client::listen(){
    char buf[MAX_BUFFER];
    if(_socket == _socketTcp){
        read(_socket, buf, MAX_BUFFER);
        std::cout << "TCP received result:\n" << buf << "\n\n" << std::endl;
    } else {
        recv(_socket, buf, MAX_BUFFER, 0);
        std::cout << "UDP received result:\n" << buf << "\n\n" << std::endl;
    }
}

bool Client::send(const char* bytes, const size_t bufLen,
            const int socket, const sockaddr* dest){
    // std::cout << "trying to send " << bytes << std::endl;
    int sent = ::send(socket, bytes, bufLen, 0);
    // std::cout << "sent " << bytes << std::endl;
    return sent == (int)bufLen;
}

void Client::sendMesg(){
    _socket = (_socketTcp > -1) ? _socketTcp : _socketUdp;
    std::string message;
    std::cout << "Compose a message for server (\"stop\" for stop loop):" << std::endl;
    std::getline(std::cin, message);
    while(message.compare("stop")){
        send(message.c_str(), message.length()+1, _socket, nullptr);
        listen();
        std::cout << "Compose a message for server (\"stop\" for stop loop):" << std::endl;
        message.clear();
        std::cin.clear();
        std::getline(std::cin, message);
    }
    close(_socket);
    shutdown(_socket, SHUT_RDWR);
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
    std::cout << "Using port " << port << "\n" << std::endl;
    Client client;
    client.bind(port, type, Device::DevClient);
    client.sendMesg();
    return 0;
}
