#include "Reactor.h"
#include "Connection.h"
#include <unistd.h>
#include <cerrno>
#include <string>


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

Reactor::Reactor(int maxEvents, int i)
{
    epfd = epoll_create(1);
    events.resize(maxEvents);
    capcity = maxEvents;
    count = 0;
    stop = false;
    no = i;
}

int Reactor::push(int _fd)
{
    // lock_guard<mutex> lg(mtx);
    epoll_event ev;
    ev.data.fd = _fd;
    ev.events = EPOLLIN | EPOLLET;  //设置为边缘触发
    count++;
    connections[_fd] = make_shared<Connection>(_fd);
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
            shared_ptr<Connection> connection = connections[fd];
            if(events[i].events & EPOLLIN)
            {
                connection->handleRead(this);
            }

            if(events[i].events & EPOLLOUT)
            {
                connection->handleWrite();
            }

            if(events[i].events & (EPOLLERR | EPOLLHUP))
            {
                // 错误或关闭处理
            }

        }

    }
}
