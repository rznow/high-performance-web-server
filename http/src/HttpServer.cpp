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
    if(request.getMethod() == "GET")
        return handleGet(request);
    else if(request.getMethod() == "POST")
    {
        return handlePost(request);
    }
    HttpResponse resp;
    resp.setStatus(405, "Method Not Allowed");
    resp.setHeader("Content-Type", "text/plain");
    resp.setBody("Unsupported HTTP Method");

    return resp;
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
    HttpResponse resp;
    if(path == "www/404.html")
    {
        resp.setStatus(404, "Not Found");
    }else
    {
        resp.setStatus(200, "OK");
    }

    resp.setHeader("Content-Type", getContentType(path));
    resp.setHeader("Content-Length", std::to_string(body.size()));
    resp.setBody(body);

    return resp;
}
HttpResponse HttpServer::handlePost(const HttpRequest& request)
{
    if(request.getPath() == "/login")   return login(request);
    else if(request.getPath() == "/register") return registerUser(request);
}
HttpResponse HttpServer::login(const HttpRequest& request)
{
    std::unordered_map<std::string, std::string> kv;

    std::string body = request.getBody();

    size_t start = 0;
    while(start < body.size())
    {
        size_t eq = body.find('=', start);
        size_t amp = body.find('&', start);

        if(eq == std::string::npos)
            break;

        std::string key = body.substr(start, eq - start);

        std::string value;
        if(amp == std::string::npos)
        {
            value = body.substr(eq + 1);
            kv[key] = value;
            break;
        }
        else
        {
            value = body.substr(eq + 1, amp - eq - 1);
            kv[key] = value;
            start = amp + 1;
        }
    }

    std::string name = kv["username"];
    std::string password = kv["password"];
    std::cout<<" username: "<<name<<std::endl;
    std::cout<<" password: "<<password<<std::endl;

    int result;

    {
        MySQL mysql;
        mysql.connect();
        result = mysql.loginSQL(name, password);
    }
    std::cout<<" result= "<<result<<std::endl;
    HttpResponse resp;
    if(result == 1)
    {
        resp.setStatus(200, "OK");
        body = R"({"code":0,"msg":"login success"})";
    }
    else if(result == 0)
    {
        resp.setStatus(401, "Unauthorized");
        body = R"({"code":1002,"msg":"wrong password"})";
    }
    else
    {
        resp.setStatus(404, "Unauthorized");
        body = R"({"code":1001,"msg":"user not exist"})";
    }
    resp.setBody(body);
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Content-Length", std::to_string(body.size()));
    return resp;
}
HttpResponse HttpServer::registerUser(const HttpRequest& request)
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