#include "network/Connection.h"
#include "network/Reactor.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "thread/ThreadPool.hpp"
#include "http/HttpServer.h"
#include <iostream>



//--------------------------------------------Buffer类--------------------------------------------//
size_t Buffer::size()
{
    return buffer.length();
}
          
bool Buffer::empty()
{
    return buffer.length()==0;
}

void Buffer::append(const char* s, size_t len)
{
    buffer.append(s, len);
}

void Buffer::append(const std::string& s)
{
    buffer.append(s);
}

void Buffer::retrieve(size_t len)
{
    buffer.erase(0, len);
}
        
void Buffer::retrieveAll()
{
    buffer.clear();
}

const std::string& Buffer::data() const
{
    return buffer;
}

//--------------------------------------------Connection类--------------------------------------------//
Connection::Connection(int _fd,ThreadPool* _pool):fd(_fd),connected(true),pool(_pool){};

void Connection::handleRead(Reactor* reactor)
{
    char buffer[4096];

    while(true)
    {
        int n = read(fd, buffer, sizeof(buffer));
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

            reactor->pop(fd);                   //其他异常
            return;
        }
    }

    


    //判断是否为完整HTTP请求
    while(HttpRequest::isRequestComplete(inputbuffer))
    {
        // auto t1 = Clock::now();
        HttpRequest request;
        request.parseRequest(inputbuffer);
        auto self = shared_from_this();
        // auto t2 = Clock::now();

        // std::cout<<"parse request: "<<t2-t1<<std::endl;
        // if(request.getMethod() == "GET")
        // {
        //     HttpServer httpServer;
        //     HttpResponse resp = httpServer.handleRequest(request);        
        //     // //注册发送事件(需要在主业务逻辑中)
        //     reactor->enResponse([self, resp = std::move(resp), reactor]{
        //         self->outputbuffer.append(resp.toString());

        //         reactor->enableWrite(self->fd);
        //     }); 
        // }else if(request.getMethod() == "POST")
        // {
        //     pool->enqueue([request, self, reactor] 
        //         {
        //             HttpServer httpServer;
        //             HttpResponse resp = httpServer.handleRequest(request);
        //             // //注册发送事件(需要在主业务逻辑中)
        //             reactor->enResponse([self, resp = std::move(resp), reactor]{
        //                 self->outputbuffer.append(resp.toString());

        //                 reactor->enableWrite(self->fd);
        //             }); 
        //         }
        //     );
        // }
        // auto enqueueTime = Clock::now();
        pool->enqueue([request, self, reactor] 
            {
                // auto t1 = Clock::now();
                // std::cout<<"queue wait:"
                //     <<t1-enqueueTime
                //     <<std::endl;
                HttpServer httpServer;
                HttpResponse resp = httpServer.handleRequest(request);
                // //注册发送事件(需要在主业务逻辑中)
                reactor->enResponse([self, resp = std::move(resp), reactor]{
                    // auto t1 = Clock::now();

                    self->outputbuffer.append(
                        std::move(resp.toString())
                    );

                    reactor->enableWrite(self->fd);


                    // auto t2 = Clock::now();

                    // cout<<"response queue:"<<t2-t1<<endl;
                }); 
                // auto t2 = Clock::now();

                // std::cout<<"http server: "<<t2-t1<<std::endl;
            }
        );
        
    }
}

void Connection::handleWrite(Reactor* reactor)
{
    //处理对应连接的写入(ET)
    // cout << outputbuffer.data() <<endl;
    // auto t1 = Clock::now();
    while(!outputbuffer.empty())
    {
        int n = write(fd, outputbuffer.data().c_str(), outputbuffer.size());
        if(n > 0)
        {
            outputbuffer.retrieve(n);
        }else
        {
            if(errno == EWOULDBLOCK ||
                errno == EAGAIN) break;
            
            reactor->pop(fd);                   //其他异常
            return;
        }
    }
    if (outputbuffer.empty())
    reactor->disableWrite(fd);

    // auto t2 = Clock::now();

    // std::cout<<"write: "<<t2-t1<<std::endl;
}

Connection::~Connection()
{
    close(fd);
}