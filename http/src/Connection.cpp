#include "Connection.h"
#include "Reactor.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ThreadPool.hpp"
#include <iostream>
#include <fstream>

//读文件
std::string readFile(std::string& filename)
{
    std::ifstream ifs(filename, std::ios::binary);

    if (!ifs.is_open())
    {
        filename = "www/404.html";
        return readFile(filename);
    }
    //拷贝整个文件
    return std::string(
        std::istreambuf_iterator<char>(ifs),
        std::istreambuf_iterator<char>()
    );
}

std::string getContentType(string path)
{
    if(path.ends_with(".html"))
        return "text/html";

    if(path.ends_with(".css"))
        return "text/css";

    if(path.ends_with(".js"))
        return "application/javascript";

    if(path.ends_with(".png"))
        return "image/png";

    if(path.ends_with(".jpg"))
        return "image/jpeg";

    if(path.ends_with(".jpeg"))
        return "image/jpeg";

    return "text/plain";
}

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

    //判断是否为完整GET请求
    while(HttpRequest::isRequestComplete(inputbuffer))
    {
        HttpRequest httprequest;
        httprequest.parseRequest(inputbuffer);

        //获取请求后,处理请求
        std::string path = httprequest.getPath();

        if(path == "/")
        {
            path = "/index.html";
        }
        
        pool->enqueue([this,path,reactor]{
            std::string fp = "www" + path;
            string body = readFile(fp);

            //服务器回应
            HttpResponse httpresponse;
            if(fp == "")
            {
                httpresponse.setStatus(404, "Not Found");
            }else
            {
                httpresponse.setStatus(200, "OK");
            }
            httpresponse.setHeader("Content-Type", "text/html");
            httpresponse.setBody(body);


            // //注册发送事件(需要在主业务逻辑中)
            // outputbuffer.append(httpresponse.toString());

            // reactor->enableWrite(fd);
        });


        
    }

    
}

void Connection::handleWrite(Reactor* reactor)
{
    //处理对应连接的写入(ET)
    // cout << outputbuffer.data() <<endl;
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
}

Connection::~Connection()
{
    close(fd);
}