#pragma once

#include <unordered_map>
#include <string>
class HttpRequest;
struct UploadFile
{
    std::string name;        // avatar
    std::string filename;    // a.png
    std::string contentType; // image/png
    std::string content;     // 二进制
};

class MultipartParser
{
public:
    bool parse(const HttpRequest& request);

    UploadFile getFile(const std::string& name);

private:
    std::unordered_map<std::string, UploadFile> files;
};