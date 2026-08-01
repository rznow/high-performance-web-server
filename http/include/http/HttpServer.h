#ifndef HTTPSERVER_H
#define HTTPSERVER_H

class HttpRequest;
class HttpResponse;
class HttpServer
{
public:
    HttpResponse handleRequest(const HttpRequest& request);

private:
    HttpResponse handleGet(const HttpRequest& request);
    HttpResponse handlePost(const HttpRequest& request);
    HttpResponse handleDel(const HttpRequest& request);
    HttpResponse handlePut(const HttpRequest& request);

    //POST
    HttpResponse login(const HttpRequest& request);
    HttpResponse registerUser(const HttpRequest& request);
    HttpResponse avatar(const HttpRequest& request);
    HttpResponse postCreate(const HttpRequest& request);
    HttpResponse commentCreate(const HttpRequest& request);
    HttpResponse post_like(const HttpRequest& request);

    //GET
    HttpResponse index(const HttpRequest& request);
    HttpResponse profile(const HttpRequest& request);
    HttpResponse posts(const HttpRequest& request);
    HttpResponse comments(const HttpRequest& request);
    HttpResponse post(const HttpRequest& request);
    
    //DEL

    //PUT
    
};

#endif