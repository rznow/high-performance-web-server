#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <unordered_map>

class Buffer;

class HttpRequest
{
    private:
        //请求行
        std::string method;
        std::string path;
        std::string version;
        //请求头
        std::unordered_map<std::string,std::string> headers;

        //请求体
        std::string body;

    public:
        static bool isRequestComplete(const Buffer& buffer);

        void parseRequest(Buffer& buffer);

        void setMethod(const std::string& _method);

        void setPath(const std::string& _path);

        void setVersion(const std::string& _version);

        void setBody(const std::string& _body);

        std::string getHeader(const std::string& key) const;
        
        const std::string& getMethod() const;

        const std::string& getPath() const;

        const std::string& getVersion() const;

        const std::string& getBody() const;

        void print() const;
};

#endif