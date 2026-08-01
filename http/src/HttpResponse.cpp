#include "http/HttpResponse.h"
#include <iostream>

//--------------------------------------------HttpResponse类--------------------------------------------//
void HttpResponse::setStatus(int code, const std::string& msg)
{
    statusCode = code;
    statusMessage = msg;
}
void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
    headers[key] = value;
}
void HttpResponse::setBody(const std::string& _body)
{
    body = _body;
}

std::string HttpResponse::toString() const
{
    std::string res;


    res += "HTTP/1.1 ";
    res += std::to_string(statusCode);
    res += " ";
    res += statusMessage;
    res += "\r\n";


    for (const auto& [key, value] : headers)
    {
        res += key;
        res += ": ";
        res += value;
        res += "\r\n";
    }

    res += "\r\n";
    res += body;

    // std::cout<<res<<std::endl;

    return res;
}

HttpResponse HttpResponse::JsonResponse(const nlohmann::json& j)
{
    HttpResponse resp;

    resp.setStatus(200, "OK");
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Connection", "keep-alive");
    resp.setHeader("Content-Length", std::to_string(j.dump().size()));
    resp.setBody(j.dump());

    return resp;
}