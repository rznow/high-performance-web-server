#include "HttpServer.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
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
    if(request.getMethod() == "GET")
        return handleGet(request);
    else if(request.getMethod() == "POST")
    {
        if(request.getPath() == "/login")   return login(request);
        else if(request.getPath() == "/register") return registerUser(request);
    }

    HttpResponse response;
    response.setStatus(404, "Not Found");
    return response;
}

HttpResponse HttpServer::handleGet(const HttpRequest& request)
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

    return httpresponse;
}
HttpResponse HttpServer::handlePost(const HttpRequest& request)
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

    return httpresponse;
}
HttpResponse HttpServer::login(const HttpRequest& request)
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

    return httpresponse;
}
HttpResponse HttpServer::registerUser(const HttpRequest& request)
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
    


    return httpresponse;
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