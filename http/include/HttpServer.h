#ifndef HTTPSERVER_H
#define HTTPSERVER_H

class HttpRequest;
class HttpResponse;
class HttpServer
{
public:
    HttpResponse handleRequest(const HttpRequest& request);

private:
    void handleGet(const HttpRequest& request);
    void handlePost(const HttpRequest& request);
    void login(const HttpRequest& request);
    void registerUser(const HttpRequest& request);
};

#endif