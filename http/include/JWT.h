#ifndef JWT_H
#define JWT_H

#include <string>
class UserInfo;
class JWT
{
private:
    static constexpr const char* SECRET = "2004--08--24";
public:
    static std::string createToken(int user_id);

    static bool verifyToken(const std::string& token,UserInfo& user);

};

#endif