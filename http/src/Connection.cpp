#include "Connection.h"
#include "Reactor.h"
#include <iostream>


Connection::Connection(int _fd):fd(_fd),connected(true){};


void Connection::handleRead(Reactor* reactor)
{
    char buffer[4096];

    while(true)
    {
        int n = read(fd, buffer, sizeof(buffer)-1);
        if(n > 0)
        {
            inputbuffer.append(buffer, n);
        }   
        else if(n == 0)                         //当读取到关闭请求(0)时,断掉连接
        {
            // 对端关闭
            reactor->pop(fd);
            return;
        }
        else
        {
            if(errno == EAGAIN ||
               errno == EWOULDBLOCK)    break;  //当读取不到数据后,结束读取.


            reactor->pop(fd);
            return;
        }
    }
    
    // std::cout<<inputbuffer<<std::endl;
    // inputbuffer.clear();
}

void Connection::handleWrite()
{
    //处理对应连接的写入
}

Connection::~Connection()
{
    close(fd);
}