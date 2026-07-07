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
    HttpResponse login(const HttpRequest& request);
    HttpResponse registerUser(const HttpRequest& request);
    HttpResponse index(const HttpRequest& request);
    HttpResponse profile(const HttpRequest& request);
};

#endif