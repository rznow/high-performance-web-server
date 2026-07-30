#include "redis/Redis.h"
#include <iostream>

Redis::Redis()
{
    connect();
    // if(connect())   std::cout<<" Connect success!"<<std::endl;
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

bool Redis::valid()
{
    if(c) return true;
    return false;
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

    if(reply->type != REDIS_REPLY_INTEGER)
    {
        return false;
    }

    return reply->integer == 1;
}

void Redis::delByPattern(const std::string& pattern)
{
    std::string cursor = "0";
    do{
        redisReply* reply = static_cast<redisReply*>(redisCommand(
            c,
            "SCAN %b MATCH %b COUNT 100",
            cursor.data(),
            cursor.size(),
            pattern.data(),
            pattern.size())
        );

        if(reply->type != REDIS_REPLY_ARRAY || reply->elements != 2)
        {
            freeReplyObject(reply);
            return;
        }

        cursor = std::string(reply->element[0]->str, reply->element[0]->len);

        redisReply* keys = reply->element[1];

        if(keys->type == REDIS_REPLY_ARRAY&&keys->elements > 0)
        {
            std::string cmd = "DEL";

            for(size_t i = 0;i < keys->elements;i++)
            {
                cmd += " ";
                cmd += std::string(keys->element[i]->str,keys->element[i]->len);
            }
            redisReply* delreply = static_cast<redisReply*>(redisCommand(c, cmd.c_str()));

            if(delreply) freeReplyObject(delreply);
        }

    }while(cursor != "0");
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

bool Redis::lrange(
    const std::string& key, 
    std::vector<std::string>& values,
    int start,
    int end)
{
    redisReply* reply = static_cast<redisReply*>(redisCommand(
        c,
        "LRANGE %b %d %d",
        key.data(),
        key.size(),
        start,
        end)
    );

    if(!reply) return false;

    if(reply->type!=REDIS_REPLY_ARRAY) return false;

    if(reply->elements == 0) return false;

    for(size_t i=0; i<reply->elements; i++)
    {
        values.emplace_back(reply->element[i]->str);
    }

    return true;
}

bool Redis::lpush(
    const std::string& key, 
    const std::vector<std::string>& values)
{
    std::vector<const char*> argv;
    std::vector<size_t> argvlen;

    argv.push_back("LPUSH");
    argvlen.push_back(5);

    argv.push_back(key.data());
    argvlen.push_back(key.size());
    for(auto& value : values)
    {
        argv.push_back(value.data());
        argvlen.push_back(value.size());
    }

    redisReply* reply =
        (redisReply*)redisCommandArgv(
            c,
            argv.size(),
            argv.data(),
            argvlen.data());

    bool ok = reply && reply->type != REDIS_REPLY_ERROR;

    freeReplyObject(reply);

    return ok;
}

bool Redis::rpush(
    const std::string& key, 
    const std::vector<std::string>& values)
{
    std::vector<const char*> argv;
    std::vector<size_t> argvlen;

    argv.push_back("RPUSH");
    argvlen.push_back(5);

    argv.push_back(key.data());
    argvlen.push_back(key.size());
    for(auto& value : values)
    {
        argv.push_back(value.data());
        argvlen.push_back(value.size());
    }

    redisReply* reply =
        (redisReply*)redisCommandArgv(
            c,
            argv.size(),
            argv.data(),
            argvlen.data());

    bool ok = reply && reply->type != REDIS_REPLY_ERROR;

    freeReplyObject(reply);

    return ok;
}

bool Redis::hmset(
    const std::string& key, 
    const std::unordered_map<std::string,std::string>& fields)
{
    std::vector<const char*> argv;
    std::vector<size_t> argvlen;

    argv.push_back("HSET");
    argvlen.push_back(4);

    argv.push_back(key.data());
    argvlen.push_back(key.size());

    for (auto& [field, value] : fields)
    {
        argv.push_back(field.data());
        argvlen.push_back(field.size());

        argv.push_back(value.data());
        argvlen.push_back(value.size());
    }

    redisReply* reply =
        (redisReply*)redisCommandArgv(
            c,
            argv.size(),
            argv.data(),
            argvlen.data());
    
    bool ok = reply && reply->type != REDIS_REPLY_ERROR;

    freeReplyObject(reply);

    return ok;
}

bool Redis::hmget(
    const std::string& key, 
    std::unordered_map<std::string,std::string>& fields)
{
    std::vector<const char*> argv;
    std::vector<size_t> argvlen;

    argv.push_back("HMGET");
    argvlen.push_back(5);

    argv.push_back(key.data());
    argvlen.push_back(key.size());
    for(auto& [key, value] : fields)
    {
        argv.push_back(key.data());
        argvlen.push_back(key.size());
    }

    redisReply* reply =
        (redisReply*)redisCommandArgv(
            c,
            argv.size(),
            argv.data(),
            argvlen.data());

    if(reply->type != REDIS_REPLY_ARRAY)
    {
        freeReplyObject(reply);
        return false;
    }

    int i=0;
    for(auto& [key, value] : fields)
    {
        value = reply->element[i++]->str;
        std::cout<<key<<":  "<<value<<std::endl;
    }

    return true;
}

bool Redis::hgetAll(
    const std::string& key, 
    std::unordered_map<std::string,std::string>& fields)
{
    redisReply* reply =
        (redisReply*)redisCommand(
            c,
            "HGETALL %b",
            key.data(),
            key.size());

    if(reply == nullptr)
        return false;

    if(reply->type != REDIS_REPLY_ARRAY || reply->elements == 0)
    {
        freeReplyObject(reply);
        return false;
    }

    fields.clear();

    for(size_t i = 0; i < reply->elements; i += 2)
    {
        fields.emplace(
            reply->element[i]->str,
            reply->element[i + 1]->str);
    }

    freeReplyObject(reply);
    return true;
}

RedisValue Redis::hget(
    const std::string& key, 
    const std::string& field)
{
    redisReply* reply =
        (redisReply*)redisCommand(
            c,
            "HGET %b %b",
            key.data(),
            key.size(),
            field.data(),
            field.size());

    RedisValue rv(reply);

    freeReplyObject(reply);
    return rv;
}

bool Redis::hincr(
    const std::string& key, 
    const std::string& field,
    const std::string& INCR)
{
    redisReply* reply =
        (redisReply*)redisCommand(
            c,
            "HINCRBY %b %b %b",
            key.data(),
            key.size(),
            field.data(),
            field.size(),
            INCR.data(),
            INCR.size());

    if(reply->type == REDIS_REPLY_ERROR) 
    {
        freeReplyObject(reply);
        return false;
    }

    // int res = reply->integer;//成功返回增长后结果
    freeReplyObject(reply);
    return true;
}

bool Redis::sadd(
    const std::string& key,
    const std::string& member)
{
    redisReply* reply =
        (redisReply*)redisCommand(
            c,
            "SADD %b %b",
            key.data(),
            key.size(),
            member.data(),
            member.size());

    if(reply->type == REDIS_REPLY_ERROR) 
    {
        freeReplyObject(reply);
        return false;
    }
    
    // int res = reply->integer;//成功返回1，失败返回0
    freeReplyObject(reply);
    return true;
}

bool Redis::srem(
    const std::string& key,
    const std::string& member)
{
    redisReply* reply =
        (redisReply*)redisCommand(
            c,
            "SREM %b %b",
            key.data(),
            key.size(),
            member.data(),
            member.size());

    if(reply->type == REDIS_REPLY_ERROR) 
    {
        freeReplyObject(reply);
        return false;
    }
    
    // int res = reply->integer;//成功返回1，失败返回0
    freeReplyObject(reply);
    return true;
}

bool Redis::sismember(
    const std::string& key,
    const std::string& member)
{
    redisReply* reply =
        (redisReply*)redisCommand(
            c,
            "SISMEMBER %b %b",
            key.data(),
            key.size(),
            member.data(),
            member.size());

    if(reply->type == REDIS_REPLY_ERROR) 
    {
        freeReplyObject(reply);
        return false;
    }
    
    int res = reply->integer;//成功返回1，失败返回0
    freeReplyObject(reply);
    if(res) return true;
    return false;
}

bool Redis::smembers(const std::string& key,
                     std::vector<std::string>& values)
{
    redisReply* reply = static_cast<redisReply*>(
        redisCommand(
            c,
            "SMEMBERS %b",
            key.data(),
            key.size()));

    if (reply == nullptr)
        return false;

    if (reply->type != REDIS_REPLY_ARRAY)
    {
        freeReplyObject(reply);
        return false;
    }

    values.clear();
    values.reserve(reply->elements);

    for (size_t i = 0; i < reply->elements; ++i)
    {
        redisReply* e = reply->element[i];

        if (e->type == REDIS_REPLY_STRING)
        {
            values.emplace_back(e->str, e->len);
        }
    }

    freeReplyObject(reply);
    return true;
}

bool Redis::spop(
    const std::string& key,
    std::string& value)
{
    redisReply* reply = static_cast<redisReply*>(
        redisCommand(
            c,
            "SPOP %b",
            key.data(),
            key.size()));

    if(reply->type != REDIS_REPLY_STRING)
    {
        freeReplyObject(reply);
        return false;
    }

    RedisValue rv(reply);
    value = rv.asString();
    return true;
}