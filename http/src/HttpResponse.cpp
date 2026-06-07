#include "HttpResponse.h"


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

std::string HttpResponse::toString()
{
    std::string res;

    // 1. 状态行
    res += "HTTP/1.1 ";
    res += std::to_string(statusCode);
    res += " ";
    res += statusMessage;
    res += "\r\n";

    // 2. Content-Length
    headers["Content-Length"] = std::to_string(body.size());

    if (headers.find("Content-Type") == headers.end())
    {
        headers["Content-Type"] = "text/plain";
    }

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
