#include <unistd.h>
#include <atomic>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <vector>
#include <iostream>
using namespace std;

atomic<bool> stop = false;
constexpr int backlog = 1024;
constexpr int PORT = 8080;
constexpr int MAXEVENTS = 100;

//----------------------Reactor类----------------------//
class Reactor
{
    private:
        int epfd;
        int maxEvents;
        vector<epoll_event> events;
    public:
        Reactor(): maxEvents(1)
        {
            epfd = epoll_create(1);
            events.resize(1);
        }

        Reactor(int maxEvents): maxEvents(maxEvents)
        {
            epfd = epoll_create(1);
            events.resize(maxEvents);
        }

        int push(int _fd)
        {
            epoll_event ev;
            ev.data.fd = _fd;
            ev.events = EPOLLIN;
            return epoll_ctl(epfd, EPOLL_CTL_ADD, _fd, &ev);
        }

        int wait()
        {
            return epoll_wait(epfd, events.data(), maxEvents, -1);
        }

        const epoll_event& operator[](size_t idx)
        {
            return events[idx];
        }
};

int main()
{
    //创建监听端口listen_fd
    int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);


    //允许端口重用
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    //创建服务器地址
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //绑定端口和地址
    bind(listen_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    //将socket设为被动监听
    listen(listen_fd, backlog);

    Reactor master;
    master.push(listen_fd);


    while(!stop)
    {
        //主reactor中的epoll轮询
        int nready = master.wait();
        if( nready && master[0].data.fd == listen_fd)
        {
            //处理新连接
            sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int new_fd = accept4(listen_fd, (sockaddr*)&client_addr, &len, SOCK_NONBLOCK);

            if(new_fd > 0)
            {
                //分给子reactor进行处理
                const char msg[] = "connect to server!\n";
                write(new_fd, msg, sizeof(msg));
            }
        }

    }




}
