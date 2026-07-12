/**
 * @file HttpServer.cpp
 * @brief 处理Http请求(HttpRequest),生成Http响应
 *
 * 实现请求解析、响应生成等功能。
 *
 * @author rznow
 * @date 2026-06-10
 */

#include "http/HttpServer.h"
#include "http/HttpResponse.h"
#include "http/HttpRequest.h"
#include "mysql/MySQL.h"
#include "mysql/MySQLPool.h"
#include "JWT.h"
#include "common/UserInfo.h"
#include "common/Post.h"
#include "service/PostService.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
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
    }else if(request.getMethod() == "DELETE")
    {
        return handleDel(request);
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

    if(path == "/profile")
    {
        return profile(request);
    }else if(path.starts_with("/posts"))
    {
        return posts(request);
    }else if(path.starts_with("/post.html"))
    {
        return index(request);
    }else if(path.starts_with("/post"))
    {
        return post(request);
    }

    return index(request);
}
HttpResponse HttpServer::handlePost(const HttpRequest& request)
{
    std::string path = request.getPath();
    if(path == "/login")   return login(request);
    else if(path == "/register") return registerUser(request);
    else if(path.starts_with("/post")) 
    {
        if(path.ends_with("/like")) return post_like(request);
        return postCreate(request);
    }
    
    HttpResponse resp;
    resp.setStatus(404, "Not Found");
    resp.setHeader("Content-Type", "application/json");
    resp.setBody(R"({"code":404,"msg":"route not found"})");
    return resp;
}


/*
    



*/
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
    UserInfo user;
    {
        auto mysql = MySQLPool::getInstance().getConnection();
        result = mysql->loginSQL(name, password, user);
        // MySQL mysql;
        // mysql.connect();
        // result = mysql.loginSQL(name, password);
    }

    
    HttpResponse resp;
    json j;
    if(result == 1)
    {
        auto token = JWT::createToken(user);
        resp.setStatus(200, "OK");
        j["code"] = 0;
        j["msg"] = "login success";
        j["token"] = token;

        body = j.dump();
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
    std::unordered_map<std::string, std::string> kv;

    std::string body = request.getBody();

    size_t start = 2;
    while(start < body.size())
    {
        size_t eq = body.find(R"(":")", start);
        size_t amp = body.find(R"(",")", start);

        if(eq == std::string::npos)
            break;

        std::string key = body.substr(start, eq - start);

        std::string value;
        if(amp == std::string::npos)
        {
            value = body.substr(eq + 3, body.size()-2-eq-3);
            kv[key] = value;
            break;
        }
        else
        {
            value = body.substr(eq + 3, amp - eq - 3);
            kv[key] = value;
            start = amp + 3;
        }
    }

    std::string name = kv["username"];
    std::string password = kv["password"];
    // std::string confirm_password = kv["confirm_password"];
    std::cout<<" username: "<<name<<std::endl;
    std::cout<<" password: "<<password<<std::endl;
    // std::cout<<" confirm_password: "<<confirm_password<<std::endl;

    int result;
    HttpResponse resp;
    // if(password == confirm_password)
    // {
    //数据库
    auto mysql = MySQLPool::getInstance().getConnection();
    result = mysql->registerSQL(name, password);
    // std::cout<<" result: "<<result<<std::endl;

    if(result == 1)
    {
        resp.setStatus(200, "OK");
        body = R"({"code":0,"msg":"register success"})";
    }
    else if(result == 0)
    {
        resp.setStatus(401, "Unauthorized");
        body = R"({"code":1003,"msg":"用户已存在"})";
    }
    resp.setBody(body);
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Content-Length", std::to_string(body.size()));
    return resp;
    // }
    
    // resp.setStatus(401, "Unauthorized");
    // body = R"({"code":1003,"msg":"wrong confirm_password"})";
    // resp.setBody(body);
    // resp.setHeader("Content-Type", "application/json");
    // resp.setHeader("Content-Length", std::to_string(body.size()));
    // return resp;

}

HttpResponse HttpServer::post_like(const HttpRequest& request)
{
    std::string path = request.getPath();

    size_t pos = path.find("post/")+5;
    size_t end = path.find("/like");
    int post_id = std::stoi(path.substr(pos, end-pos));

    std::string auth = request.getHeader("Authorization");

    if(auth.starts_with("Bearer "))
    {
        auth = auth.substr(7);
    }
    HttpResponse resp;
    UserInfo user;
    json j;
    if(!JWT::verifyToken(auth, user))
    {
        j["code"] = 1003;
        j["msg"] = "token invalid";


        resp.setBody(j.dump());
        resp.setHeader("Content-Type", "application/json");
        resp.setHeader("Content-Length", std::to_string(j.dump().size()));
        return resp;
    }
    
    int like_count = PostService::getInstance().like(post_id, user.user_id);
    if(like_count != -1)
    {
        j["code"] = 0;
        j["like_count"] = like_count;
    }else
    {
        j["code"] = 1001;
        j["msg"] = "点赞失败";
    }
    
    
    resp.setStatus(200, "OK");
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Content-Length", std::to_string(j.dump().size()));
    resp.setBody(j.dump());
    std::cout<<j.dump()<<std::endl;
    return resp;
}

HttpResponse HttpServer::postCreate(const HttpRequest& request)
{
    HttpResponse resp;
    json j;
    std::string auth = request.getHeader("Authorization");

    if(auth.starts_with("Bearer "))
    {
        auth = auth.substr(7);
    }

    UserInfo user;
    if(!JWT::verifyToken(auth, user))
    {
        j["code"] = 1003;
        j["msg"] = "token invalid";


        resp.setBody(j.dump());
        resp.setHeader("Content-Type", "application/json");
        resp.setHeader("Content-Length", std::to_string(j.dump().size()));
        return resp;

    }
    Post p;
    p.user_id = user.user_id;
    p.author = user.user_name;
    std::string body = request.getBody();

    size_t pos = body.find(R"("title":)")+9;
    size_t end = body.find(R"("content":)");

    p.title = body.substr(pos, end-pos-2);
    pos = end + 11;
    p.content = body.substr(pos, body.size()-pos-2);

    PostService::getInstance().put(p);

    j["code"] = 0;
    j["msg"] = "post success";
    j["user_id"] = user.user_id;
    j["user_name"] = user.user_name;
    resp.setStatus(200, "OK");
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Content-Length", std::to_string(j.dump().size()));
    resp.setBody(j.dump());
    std::cout<<j.dump()<<std::endl<<std::endl;
    return resp;

}

HttpResponse HttpServer::index(const HttpRequest& request)
{
    std::string path = request.getPath();
    size_t pos = path.find('?');
    path = path.substr(0, pos);
    if(path == "/") path = "/index.html";
    path = "www" + path;

    // std::cout<<"Path:\t"<<path<<std::endl;
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

HttpResponse HttpServer::profile(const HttpRequest& request)
{
    //服务器回应
    HttpResponse resp;
    json j;
    std::string auth = request.getHeader("Authorization");

    if(auth.starts_with("Bearer "))
    {
        auth = auth.substr(7);
    }

    UserInfo user;
    if(!JWT::verifyToken(auth, user))
    {
        j["code"] = 1003;
        j["msg"] = "token invalid";

        resp.setStatus(200, "OK");
        resp.setBody(j.dump());
        resp.setHeader("Content-Type", "application/json");
        resp.setHeader("Content-Length", std::to_string(j.dump().size()));
        return resp;

    }


    j["code"] = 0;
    j["user_id"] = user.user_id;
    j["user_name"] = user.user_name;

    resp.setStatus(200, "OK");
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Content-Length", std::to_string(j.dump().size()));
    resp.setBody(j.dump());
    std::cout<<j.dump()<<std::endl<<std::endl;
    return resp;
}

HttpResponse HttpServer::posts(const HttpRequest& request)
{
    std::string path = request.getPath();
    size_t pos = 7;

    size_t start = path.find('=')+1;
    size_t end = path.find('&');
    size_t page = std::stoi(path.substr(start, end-start));
    pos = end + 1;
    start = path.find('=', pos)+1;
    size_t size = std::stoi(path.substr(start));

    std::cout<<"page:\t"<<page<<std::endl;
    std::cout<<"size:\t"<<size<<std::endl;

    HttpResponse resp;
    json j;

    std::vector<Post> posts = PostService::getInstance().getPosts(page, size);

    j["code"] = 0;
    json post_array = json::array();
    for(auto &i: posts)
    {
        post_array.push_back({
            {"post_id",         i.post_id},
            {"user_id",         i.user_id},
            {"author",          i.author},
            {"title",           i.title},
            {"content",         i.content},
            {"like_count",      i.like_count},
            {"comment_count",   i.comment_count},
            {"view_count",      i.view_count},
            
            
            {"time", i.create_time}
        });
    }
    j["posts"] = post_array;

    resp.setStatus(200, "OK");
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Content-Length", std::to_string(j.dump().size()));
    resp.setBody(j.dump());


    return resp;
}

HttpResponse HttpServer::post(const HttpRequest& request)
{
    std::string path = request.getPath();

    size_t pos = path.find('=')+1;
    int id = std::stoi(path.substr(pos));

    HttpResponse resp;
    json j;
    Post p;
    
    std::string auth = request.getHeader("Authorization");

    if(auth.starts_with("Bearer "))
    {
        auth = auth.substr(7);
    }
    UserInfo user;
    JWT::verifyToken(auth, user);
    if(!PostCache::getInstance().get(id, p)&&!PostService::getInstance().get(id, p))
    {

        j["code"] = 1001;
        j["msg"] = "post not found";

    }else
    {
        j["code"] = 0;
        j["post"]["post_id"]        = p.post_id;
        j["post"]["user_id"]        = p.user_id;
        j["post"]["author"]         = p.author;
        j["post"]["title"]          = p.title;
        j["post"]["content"]        = p.content;
        j["post"]["like_count"]     = p.like_count;
        j["post"]["comment_count"]  = p.comment_count;
        j["post"]["view_count"]     = p.view_count;
        j["post"]["time"]           = p.create_time;
        j["post"]["liked"]          = PostService::getInstance().liked(id, user.user_id);
    }
    p.print();
    
    resp.setStatus(200, "OK");
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Content-Length", std::to_string(j.dump().size()));
    resp.setBody(j.dump());
    std::cout<<j.dump()<<std::endl;
    return resp;
}

HttpResponse HttpServer::handleDel(const HttpRequest& request)
{
    //服务器回应
    HttpResponse resp;
    json j;
    std::string auth = request.getHeader("Authorization");
    if(auth.starts_with("Bearer "))
    {
        auth = auth.substr(7);
    }

    UserInfo user;
    if(!JWT::verifyToken(auth, user))
    {
        j["code"] = 1003;
        j["msg"] = "token invalid";

        resp.setStatus(200, "OK");
        resp.setBody(j.dump());
        resp.setHeader("Content-Type", "application/json");
        resp.setHeader("Content-Length", std::to_string(j.dump().size()));
        return resp;

    }

    int id = std::stoi(request.getPath().substr(7));

    if(PostService::getInstance().delPost(id))
    {
        j["code"] = 0;
        j["user_id"] = user.user_id;
        j["msg"] = "delete success";
    }else
    {
        j["code"] = 1001;
        j["user_id"] = user.user_id;
        j["msg"] = "delete fail";
    }

    resp.setStatus(200, "OK");
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Content-Length", std::to_string(j.dump().size()));
    resp.setBody(j.dump());
    std::cout<<j.dump()<<std::endl<<std::endl;
    return resp;
}