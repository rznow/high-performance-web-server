#include "redis/Redis.h"
#include "redis/RedisValue.h"
#include <iostream>

Redis::Redis()
{
    if(connect())   std::cout<<" Connect success!"<<std::endl;
}

Redis::~Redis()
{
    // std::cout << "free: " << c << std::endl;
    if(c)
    {
        redisFree(c);
    }
}

bool Redis::connect(const std::string& host, int port)
{
    c = redisConnect(host.c_str(), port);

    // Check if the context is null or if a specific
    // error occurred.
    if (c == nullptr || c->err) {
        if (c != nullptr) {
            std::cout<<"Error: "<<c->errstr<<std::endl;
            // handle error
        } else {
            std::cout<<"Can't allocate redis context\n"<<std::endl;
        }

        return false;
    }
    return true;
}

bool Redis::set(const std::string& key, const std::string& value)
{
    redisReply* reply = static_cast<redisReply*>(redisCommand(
        c,
        "SET %b %b",
        key.data(),
        key.size(),
        value.data(),
        value.size())
        );


    if (reply == nullptr)
    {
        return false;
    }

    bool ok = false;
    if (reply->type == REDIS_REPLY_STATUS)
    {
        std::cout << "reply: " << reply->str << std::endl;

        ok = (std::string(reply->str) == "OK");
    }
    else
    {
        std::cout << "Redis Error" << std::endl;
    }
    freeReplyObject(reply);

    return ok;
}

RedisValue Redis::get(const std::string& key)
{
    redisReply* reply = static_cast<redisReply*>(redisCommand(
        c,
        "GET %b",
        key.data(),
        key.size())
        );

    RedisValue rv(reply);
    freeReplyObject(reply);

    return rv;
}

bool Redis::del(const std::string& key)
{
    redisReply* reply = static_cast<redisReply*>(redisCommand(
        c,
        "DEL %b",
        key.data(),
        key.size())
        );

    RedisValue rv(reply);
    freeReplyObject(reply);

    return true;
}

bool Redis::expire(const std::string& key, int seconds)
{
    redisReply* reply = static_cast<redisReply*>(redisCommand(
        c,
        "EXPIRE %b %d",
        key.data(),
        key.size(),
        seconds)
        );

    RedisValue rv(reply);
    freeReplyObject(reply);

    return true;
}

RedisValue Redis::incr(const std::string& key)
{
    redisReply* reply = static_cast<redisReply*>(redisCommand(
        c,
        "INCR %b",
        key.data(),
        key.size())
        );
    RedisValue rv(reply);
    freeReplyObject(reply);

    return rv;
}