#include "Acceptor.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

Acceptor::Acceptor(int port)
{
    //创建监听端口listen_fd
    listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);


    //允许端口重用
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    //创建服务器地址
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //绑定端口和地址
    bind(listenfd, (sockaddr*)&server_addr, sizeof(server_addr));

    //将socket设为被动监听
    listen(listenfd, backlog);

}

int Acceptor::fd() const{   return listenfd;}

int Acceptor::acceptConnection()
{
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    return accept4(listenfd,
        (sockaddr*)&client_addr,
        &len,
        SOCK_NONBLOCK
    );
}

Acceptor::~Acceptor()
{
    close(listenfd);
};