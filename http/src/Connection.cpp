#include "Connection.h"
#include "Reactor.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
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
Connection::Connection(int _fd):fd(_fd),connected(true){};

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

    //判断是否为完整GET请求
    while(HttpRequest::isRequestComplete(inputbuffer))
    {
        HttpRequest httprequest;
        httprequest.parseRequest(inputbuffer);

        //获取请求后,处理请求
        /*
            暂时没后续
        
        
        
        */


        //服务器回应
        HttpResponse httpresponse;
        string statusMessage = "OK";
        httpresponse.setStatus(200, statusMessage);
        httpresponse.setHeader("Content-Type", "text/html");
        httpresponse.setBody("hello");

        //注册发送事件
        outputbuffer.append(httpresponse.toString());

        reactor->enableWrite(fd);
    }

    
}

void Connection::handleWrite(Reactor* reactor)
{
    //处理对应连接的写入(ET)
    // cout << outputbuffer.data() <<endl;
    while(!outputbuffer.empty())
    {
        size_t n = write(fd, outputbuffer.data().c_str(), outputbuffer.size());
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
}

Connection::~Connection()
{
    close(fd);
}