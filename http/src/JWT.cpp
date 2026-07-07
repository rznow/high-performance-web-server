#include "JWT.h"
#include <jwt-cpp/jwt.h>
#include <chrono>
#include "common/UserInfo.h"
#include <string>
#include <iostream>

std::string JWT::createToken(const UserInfo& user)
{
    auto token = jwt::create()
        .set_type("JWT")
        .set_issuer("rznow")
        .set_payload_claim(
            "user_id",
            jwt::claim(std::to_string(user.user_id)))
        .set_payload_claim(
            "user_name", 
            jwt::claim(user.user_name))
        .set_expires_at(
            std::chrono::system_clock::now() +
            std::chrono::hours(24))
        .sign(jwt::algorithm::hs256{SECRET});

    return token;
}

bool JWT::verifyToken(const std::string& token,UserInfo& user)
{
    try
    {
        auto decoded = jwt::decode(token);

        auto verifier =
            jwt::verify()
                .allow_algorithm(
                    jwt::algorithm::hs256{SECRET})
                .with_issuer("rznow");

        verifier.verify(decoded);
        user.user_id = stoi(
            decoded.get_payload_claim("user_id")
                   .as_string());

        user.user_name =
            decoded.get_payload_claim("user_name")
                   .as_string();

        return true;
    }
    catch(...)
    {
        return false;
    }
}

