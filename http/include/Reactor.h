#ifndef REACTOR_H
#define REACTOR_H

#include <atomic>
#include <vector>
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>

#include "Connection.h"
using namespace std;

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
        // mutex mtx;
    public:
        Reactor();

        Reactor(int maxEvents, int i);

        int push(int _fd);

        int wait();

        const epoll_event& operator[](size_t idx);

        int get_count() const;

        ~Reactor();
        
        void workloop();
};

#endif