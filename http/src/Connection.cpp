#include "Connection.h"
#include "Reactor.h"
#include "HttpRequest.h"
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

void Buffer::append(string s)
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
            inputbuffer.append(buffer);
        }   
        else if(n == 0)                         //当读取到关闭请求(0)时,断掉连接
        {
            // 对端关闭
            reactor->pop(fd);
            // cout<<inputbuffer.data()<<endl;
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
    cout<<inputbuffer.data()<<endl;
    if(HttpRequest::isRequestComplete(inputbuffer))
    {
        HttpRequest httprequest;
        httprequest.parseRequest(inputbuffer);

        //获取请求后,处理请求
        cout<<httprequest.getMethod()<<endl;
        cout<<httprequest.getPath()<<endl;
        cout<<httprequest.getVersion()<<endl;
    }else
    {

    }

    
}

void Connection::handleWrite()
{
    //处理对应连接的写入
}

Connection::~Connection()
{
    close(fd);
}