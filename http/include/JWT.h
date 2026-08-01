#ifndef JWT_H
#define JWT_H

#include <jwt-cpp/jwt.h>
#include <chrono>
#include "common/UserInfo.h"
#include <string>
#include <iostream>
class UserInfo;
class JWT
{
private:
    static constexpr const char* SECRET = "2004--08--24";
public:
    static std::string createToken(const UserInfo& user);

    static bool verifyToken(const std::string& token,UserInfo& user);

};

#endif