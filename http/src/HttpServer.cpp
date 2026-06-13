#include "HttpServer.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "MySQL.h"
#include <fstream>
#include <iostream>

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

std::string getContentType(std::string path)
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


HttpResponse HttpServer::handleRequest(const HttpRequest& request)
{
    std::string path = request.getPath();

    if(path == "/")
    {
        path = "/index.html";
    }
    path = "www" + path;
    std::string body = readFile(path);

    //服务器回应
    HttpResponse httpresponse;
    if(path == "www/404.html")
    {
        httpresponse.setStatus(404, "Not Found");
    }else
    {
        httpresponse.setStatus(200, "OK");
    }
    httpresponse.setHeader("Content-Type", getContentType(path));
    httpresponse.setHeader("Content-Length", std::to_string(body.size()));
    httpresponse.setBody(body);

    if(request.getMethod() == "GET")
        handleGet(request);
    else if(request.getMethod() == "POST")
    {
        handlePost(request);
    }

    return httpresponse;
}

void HttpServer::handleGet(const HttpRequest& request)
{
    
}
void HttpServer::handlePost(const HttpRequest& request)
{
    if(request.getPath() == "/login")   login(request);
    else if(request.getPath() == "/register") registerUser(request);
}
void HttpServer::login(const HttpRequest& request)
{
    std::string body = request.getBody();
    size_t pos = body.find('&');
    size_t name_pos = body.find('=');
    std::string name = body.substr(name_pos+1, pos-name_pos-1);

    pos+=10;

    std::string password = body.substr(pos);

    //数据库
    MySQL mysql;
    mysql.connect();

    mysql.loginSQL(name, password);


}
void HttpServer::registerUser(const HttpRequest& request)
{
    std::string body = request.getBody();
    size_t pos = body.find('&');
    size_t name_pos = body.find('=');
    std::string name = body.substr(name_pos+1, pos-name_pos-1);

    pos+=10;

    std::string password = body.substr(pos, body.find('&', pos)-pos);
    pos = body.find('&', pos) + 18;


    std::string confirm_password = body.substr(pos);
    std::cout<<" password: "<<password<<std::endl;
    std::cout<<" confirm_password: "<<confirm_password<<std::endl;
    if(password == confirm_password)
    {
        //数据库
        MySQL mysql;
        mysql.connect();

        mysql.registerSQL(name, password);
    }
    
}



// //获取请求后,处理请求
//         std::string path = httprequest.getPath();

//         if(path == "/")
//         {
//             path = "/index.html";
//         }
//         auto self = shared_from_this();
//         pool->enqueue([self,path,reactor]{
//             std::string fp = "www" + path;
//             string body = readFile(fp);

//             //服务器回应
//             HttpResponse httpresponse;
//             if(fp == "")
//             {
//                 httpresponse.setStatus(404, "Not Found");
//             }else
//             {
//                 httpresponse.setStatus(200, "OK");
//             }
//             httpresponse.setHeader("Content-Type", getContentType(fp));
//             httpresponse.setBody(body);
//             std::string response = httpresponse.toString();

            
//         });