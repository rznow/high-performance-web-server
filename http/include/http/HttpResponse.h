#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
class HttpResponse
{
    private:
        int statusCode;
        std::string statusMessage;
        std::unordered_map<std::string, std::string> headers;
        std::string body;

    public:
        void setStatus(int code, const std::string& msg);
        void setHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body);
        std::string toString() const;
        static HttpResponse JsonResponse(const nlohmann::json& j);
};

#endif