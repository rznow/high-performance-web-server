#include "HttpRequest.h"
#include "Connection.h"
#include <iostream>
using namespace std;

bool HttpRequest::isRequestComplete(const Buffer& buffer)
{
    //判断请求是否完整(包含GET和POST)
    size_t end = buffer.data().find("\r\n\r\n");
    if(end == std::string::npos) return false;
    end += 4;
    //判断是否为POST
    size_t pos = buffer.data().find("Content-Length:");

    //不存在请求体, 是GET请求
    if(pos == std::string::npos) return true;
    pos += 15;

    //找到content-length行的尾端
    size_t lineEnd = buffer.data().find("\r\n", pos);

    size_t length = stoi(buffer.data().substr(pos, lineEnd - pos));
    //判断总大小是否大于等于当前请求的大小
    return buffer.data().size() >= end + length; 
}

void HttpRequest::parseRequest(Buffer& buffer)
{
    size_t pos = buffer.data().find("\r\n\r\n");   //pos为第一个请求的结尾
    if(pos == std::string::npos) return;
    std::string request = buffer.data().substr(0, pos+2);
    //处理请求行
    size_t end = request.find("\r\n");
    if(end == std::string::npos)    return;
    //GET
    size_t i = 0;
    while(i < end&& request[i]==' ') i++;
    size_t begin = i;
    while(i < end&& request[i]!=' ') i++;
    setMethod(request.substr(begin,i-begin));
    cout<< "Method:\t\t" << request.substr(begin,i-begin) <<endl;
    //Path
    while(i < end&& request[i]==' ') i++;
    begin = i;
    while(i < end&& request[i]!=' ') i++;
    setPath(request.substr(begin,i-begin));
    cout<< "Path:\t\t" << request.substr(begin,i-begin) <<endl;
    //Version
    while(i < end&& request[i]==' ') i++;
    begin = i;
    while(i < end&& request[i]!=' ') i++;
    setVersion(request.substr(begin,i-begin));
    cout<< "Version:\t" << request.substr(begin,i-begin) <<endl;
    //处理请求头
    i = end + 2;
    while(i < request.size())
    {
        size_t lineEnd = request.find("\r\n", i);
        
        if(lineEnd == std::string::npos)    break;

        std::string line =  request.substr(i,lineEnd - i);

        size_t colon = line.find(':');

        if(colon != std::string::npos)
        {
            std::string key = line.substr(0, colon);

            colon++;
            while(line[colon] == ' ' && colon < lineEnd)
            {
                colon++;
            }

            std::string value = line.substr(colon);

            headers[key] = value;
        }
        i = lineEnd + 2;
    }
    
    for(auto &i: headers)
    {
        cout<<i.first<<":\t\t"<<i.second<<endl;
    }
    //存在body:
    int contentLen = 0;
    if(headers.count("Content-Length"))
    {
        contentLen = stoi(headers["Content-Length"]);
        setBody(buffer.data().substr(pos+4,contentLen));
        cout<<"Body:\t\t"<<getBody()<<endl;
    }
    cout<<endl;
    
    buffer.retrieve(pos+4+contentLen);
}

void HttpRequest::setMethod(const std::string& _method)
{
    method = _method;
}

void HttpRequest::setPath(const std::string& _path)
{
    path = _path;
}

void HttpRequest::setVersion(const std::string& _version)
{
    version = _version;
}

void HttpRequest::setBody(const std::string& _body)
{
    body = _body;
}

std::string HttpRequest::getHeader(const std::string& key)
{
    auto it = headers.find(key);

    if(it == headers.end()) return "";

    return it->second;
}

const std::string& HttpRequest::getMethod() const
{
    return method;
}

const std::string& HttpRequest::getPath() const
{
    return path;
}

const std::string& HttpRequest::getVersion() const
{
    return version;
}

const std::string& HttpRequest::getBody() const
{
    return body;
}