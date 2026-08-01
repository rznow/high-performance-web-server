#include "http/MultipartParser.h"
#include "http/HttpRequest.h"

/*
    ------boundary\r\n
    Content-Disposition: form-data; name="username"\r\n
    \r\n
    ${username}\r\n
    ------boundary\r\n
    Content-Disposition: form-data; name="avatar"; filename="avatar.png"\r\n
    Content-Type: image/png\r\n
    \r\n
    <图片二进制>\r\n
    ------boundary--\r\n
*/

bool MultipartParser::parse(const HttpRequest& request)
{
    files.clear();

    std::string contentType =
        request.getHeader("Content-Type");

    auto pos = contentType.find("boundary=");

    if(pos == std::string::npos)
        return false;

    std::string boundary =
        "--" + contentType.substr(pos + 9);

    const std::string& body =
        request.getBody();

    size_t cur = 0;

    while(true)
    {
        size_t partBegin =
            body.find(boundary, cur);

        if(partBegin == std::string::npos)
            break;

        partBegin += boundary.size();

        if(body.compare(partBegin,2,"--")==0)   //boundary--\r\n是body结尾的标识
            break;

        if(body.compare(partBegin,2,"\r\n")==0) //boundary\r\n是单个字段开始的标识
            partBegin += 2;

        //--------------------------------
        // Content-Disposition
        //--------------------------------

        size_t headerEnd =
            body.find("\r\n\r\n", partBegin);

        if(headerEnd == std::string::npos)
            break;

        std::string header =
            body.substr(partBegin,
                        headerEnd-partBegin);

        UploadFile file;

        //--------------------------------
        // name=
        //--------------------------------

        size_t namePos =
            header.find("name=\"");

        if(namePos != std::string::npos)
        {
            size_t end =
                header.find("\"",
                            namePos+6);

            file.name =
                header.substr(namePos+6,
                              end-namePos-6);
        }

        //--------------------------------
        // filename=
        //--------------------------------

        size_t filenamePos =
            header.find("filename=\"");

        if(filenamePos != std::string::npos)
        {
            size_t end =
                header.find("\"",
                            filenamePos+10);

            file.filename =
                header.substr(filenamePos+10,
                              end-filenamePos-10);
        }

        //--------------------------------
        // Content-Type
        //--------------------------------

        size_t typePos =
            header.find("Content-Type:");

        if(typePos != std::string::npos)
        {
            size_t end =
                header.find("\r\n",
                            typePos);

            file.contentType =
                header.substr(typePos+13,
                              end-typePos-13);

            while(!file.contentType.empty() &&
                  file.contentType.front()==' ')
            {
                file.contentType.erase(0,1);
            }
        }

        //--------------------------------
        // 文件内容
        //--------------------------------

        size_t dataBegin =
            headerEnd+4;

        size_t dataEnd =
            body.find(boundary,dataBegin);

        if(dataEnd==std::string::npos)
            break;

        dataEnd-=2;

        file.content =
            body.substr(dataBegin,
                        dataEnd-dataBegin);

        files[file.name]=std::move(file);

        cur=dataEnd;
    }

    return !files.empty();
}

UploadFile MultipartParser::getFile(const std::string& name)
{
    if(files.find(name) != files.end())
        return files[name];

    return {};
}