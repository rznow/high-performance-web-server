#include "JWT.h"
#include <jwt-cpp/jwt.h>
#include <chrono>
#include "common/UserInfo.h"
#include <string>

std::string JWT::createToken(int userID)
{
    auto token = jwt::create()
        .set_type("JWT")
        .set_issuer("rznow")
        .set_payload_claim(
            "userID",
            jwt::claim(std::to_string(userID)))
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

        return true;
    }
    catch(...)
    {
        return false;
    }
}

