#ifndef REACTOR_H
#define REACTOR_H

#include <atomic>
#include <vector>
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <queue>
#include <functional>
#include <mutex>
#include <sys/eventfd.h>
using namespace std;


class Connection;
class ThreadPool;
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
        unordered_map<int, shared_ptr<Connection>> connections;  
        ThreadPool* pool;
        queue<function<void()>> responses;
        mutex mtx;

        int wakeupFd;
    public:
        Reactor();

        Reactor(int maxEvents, int i, ThreadPool *_pool);

        int pushListen(int listenfd);

        int push(int _fd);

        int pop(int _fd);

        int wait();

        const epoll_event& operator[](size_t idx);

        int get_count() const;

        int get_no() const;
        
        void enResponse(function<void()> response);

        void responseLoop();

        ~Reactor();
        
        void workloop();

        void enableWrite(int fd);

        void disableWrite(int fd);
};

#endif