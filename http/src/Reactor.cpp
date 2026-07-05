#include "network/Reactor.h"
#include "network/Connection.h"
#include "http/HttpRequest.h"
#include <unistd.h>
#include <cerrno>
#include <string>
#include <iostream>


//--------------------------------------------Reactor类--------------------------------------------//

Reactor::Reactor()
{
    no = -1;
    epfd = epoll_create(1);
    events.resize(1);
    capcity = 1;
    count = 0;
    stop = false;
}

Reactor::Reactor(int maxEvents, int i, ThreadPool* _pool)
{
    epfd = epoll_create(1);
    events.resize(maxEvents);
    capcity = maxEvents;
    count = 0;
    stop = false;
    no = i;
    pool = _pool;

    //eventfd
    wakeupFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    epoll_event ev;
    ev.data.fd = 1;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, wakeupFd, &ev);
}

int Reactor::pushListen(int listenfd)
{
    epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;  //listen端口设置为默认水平触发
    count++;
    connections[listenfd] = make_shared<Connection>(listenfd, pool);
    return epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
}

int Reactor::push(int _fd)
{
    
    epoll_event ev;
    ev.data.fd = _fd;
    ev.events = EPOLLIN | EPOLLET;  //设置为边缘触发
    count++;
    connections[_fd] = make_shared<Connection>(_fd, pool);
    return epoll_ctl(epfd, EPOLL_CTL_ADD, _fd, &ev);
}

int Reactor::pop(int _fd)
{
    connections.erase(_fd);         //智能指针,引用计数归零后,Connection自动析构
    epoll_ctl(epfd, EPOLL_CTL_DEL, _fd, nullptr);
    count--;
    return 0;
}

int Reactor::wait()
{
    return epoll_wait(epfd, events.data(), capcity, -1);
}

const epoll_event& Reactor::operator[](size_t idx)
{
    return events[idx];
}

int Reactor::get_count() const {return count.load();}

int Reactor::get_no() const {return no;}

void Reactor::enResponse(function<void()> task)
{
    lock_guard<mutex> lg(mtx);
    responses.emplace(move(task));

    //唤醒wait
    uint64_t one = 1;
    write(wakeupFd, &one, sizeof(one));
}

void Reactor::responseLoop()
{
    // std::cout<<" ResponsesLoop: "<<responses.size()<<std::endl;
    while(responses.size())
    {
        lock_guard<mutex> lg(mtx);
        
        auto response = responses.front();

        responses.pop();
        response();
    }
}

Reactor::~Reactor(){ close(epfd); };

void Reactor::workloop()
{
    while(true)
    {
        // lock_guard<mutex> lg(mtx);
        int nready = wait();    //epoll_wait线程安全,epoll 内部使用内核锁（如自旋锁或互斥锁）保护其就绪队列和红黑树等数据结构，因此多个系统调用可以安全并发执行。epoll_ctl不会影响epoll_wait的执行
        for(int i=0;i < nready;i++)
        {
            int fd = events[i].data.fd;
            if(fd == wakeupFd) 
            {
                uint64_t cnt;
                while(read(wakeupFd, &cnt, sizeof(cnt)) > 0);
                
                continue;
            }
            if(connections.count(fd) )
            {
                shared_ptr<Connection> connection = connections[fd];
                if(events[i].events & EPOLLIN)
                {
                    connection->handleRead(this);
                }

                if(events[i].events & EPOLLOUT)
                {
                    connection->handleWrite(this);
                }

                if(events[i].events & (EPOLLERR | EPOLLHUP))
                {
                    // 错误或关闭处理
                }
            }
            

        }

        //处理写回
        responseLoop();

    }
}

void Reactor::enableWrite(int fd)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;

    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);

}

void Reactor::disableWrite(int fd)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;

    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}