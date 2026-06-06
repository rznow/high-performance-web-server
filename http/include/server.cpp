#include <unistd.h>
#include <atomic>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <vector>
#include <iostream>
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
using namespace std;

constexpr int backlog = 1024;
constexpr int PORT = 8080;
constexpr int MAXEVENTS = 100;
constexpr int SUBTHREAD = 3;


//--------------------------------------------Reactor类--------------------------------------------//
class Reactor
{
    private:
        int no;
        int epfd;
        int capcity;
        atomic<int> count;
        vector<epoll_event> events;
        atomic<bool> stop;
        mutex mtx;
    public:
        Reactor(): capcity(1),no(-1)
        {
            epfd = epoll_create(1);
            events.resize(1);
            count = 0;
            stop = false;
        }

        Reactor(int maxEvents, int i): capcity(maxEvents)
        {
            epfd = epoll_create(1);
            events.resize(maxEvents);
            count = 0;
            stop = false;
            no = i;
        }

        int push(int _fd)
        {
            // lock_guard<mutex> lg(mtx);
            epoll_event ev;
            ev.data.fd = _fd;
            ev.events = EPOLLIN;
            count++;
            return epoll_ctl(epfd, EPOLL_CTL_ADD, _fd, &ev);
        }

        int wait()
        {
            return epoll_wait(epfd, events.data(), capcity, -1);
        }

        const epoll_event& operator[](size_t idx)
        {
            return events[idx];
        }

        int get_count() const {return count;}

        ~Reactor(){ close(epfd); };
        
        void workloop()
        {
            while(true)
            {
                // lock_guard<mutex> lg(mtx);
                int nready = wait();    //epoll_wait线程安全,epoll 内部使用内核锁（如自旋锁或互斥锁）保护其就绪队列和红黑树等数据结构，因此多个系统调用可以安全并发执行。epoll_ctl不会影响epoll_wait的执行
                for(int i=0;i < nready;i++)
                {
                    int fd = events[i].data.fd;
                    if(events[i].events & EPOLLIN)
                    {
                        char buffer[1024];
                        int n = read(fd, buffer, sizeof(buffer));
                        if(n == 0)
                        {
                            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                            close(fd);
                            count--;
                            continue;
                        }else if(n < 0)
                        {
                            if(errno == EAGAIN || errno == EWOULDBLOCK)
                                break;

                            close(fd);
                        }else   buffer[n] = '\0';
                        string s = "来自"+to_string(no)+"子Reactor:";
                        write(fd, s.c_str(), s.length());
                        write(fd, buffer, n);
                    }

                    if(events[i].events & EPOLLOUT)
                    {
                        // 可写处理
                    }

                    if(events[i].events & (EPOLLERR | EPOLLHUP))
                    {
                        // 错误或关闭处理
                    }

                }

            }
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

    //创建子Reactor序列
    vector<unique_ptr<Reactor>> subReactors;
    vector<thread> threads;

    for(int i=0;i < SUBTHREAD; i++)
    {
        subReactors.emplace_back(make_unique<Reactor>(MAXEVENTS,i));
        threads.emplace_back([&subReactors,i]{subReactors[i]->workloop();});
    }

    
    atomic<int> next{0};
    while(1)
    {
        //主reactor中的epoll轮询
        int nready = master.wait();
        for(int i = 0; i < nready; ++i)
        {
            if(master[i].data.fd != listen_fd)
                continue;
            //处理新连接
            while(true)         //一次就绪遍历所有就绪事件
            {
                sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int new_fd = accept4(listen_fd,
                    (sockaddr*)&client_addr, 
                    &len, 
                    SOCK_NONBLOCK);

                if(new_fd < 0)
                {
                    if(errno == EAGAIN ||
                    errno == EWOULDBLOCK)
                        break;

                    perror("accept4");
                    break;
                }else if(new_fd > 0)
                {
                    //分给子reactor进行处理
                    const char message[] = "connect to server!\n";
                    write(new_fd, message, sizeof(message));


                    //通过轮询来为子Reactor添加socket端口
                    int idx = next.fetch_add(1) % SUBTHREAD;

                    subReactors[idx]->push(new_fd);

                    string msg = "分配到"+to_string(idx)+"号Reactor!\n";
                    write(new_fd, msg.c_str(), msg.length());
                }
            }
        }
    }
}
