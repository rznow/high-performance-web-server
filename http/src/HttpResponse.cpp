#include "HttpResponse.h"

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

    return res;
}
