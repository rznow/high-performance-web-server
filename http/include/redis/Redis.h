#pragma once

#include <hiredis/hiredis.h>
#include <string>
#include <unordered_map>

/* This is the reply object returned by redisCommand() 
typedef struct redisReply {
    int type; 返回值类型 
    long long integer; 当返回类型为 REDIS_REPLY_INTEGER 时 
    size_t len; 返回的字符串长度 
    char *str; 当返回值类型为 REDIS_REPLY_ERROR 和 REDIS_REPLY_STRING 
    size_t elements; 返回的数组长度 
    struct redisReply **element; 当返回值类型为 REDIS_REPLY_ARRAY 
} redisReply;
*/


class RedisValue;
class Redis
{
    private:
        redisContext *c;
    public:
        Redis();
        ~Redis();

        bool connect(
            const std::string& host = "192.168.1.8",
            int port = 6379);

        bool set(const std::string& key, const std::string& value);
        RedisValue get(const std::string& key);
        bool del(const std::string& key);
        bool expire(const std::string& key, int seconds);

        RedisValue incr(const std::string& key);

        bool hmset(
            const std::string& key, 
            const std::unordered_map<std::string,std::string>& fields);

        bool hgetAll(
            const std::string& key, 
            std::unordered_map<std::string,std::string>& fields);

        bool hincr(
            const std::string& key, 
            const std::string& field,
            const std::string& INCR);

        bool sadd(
            const std::string& key,
            const std::string& member);

        bool srem(
            const std::string& key,
            const std::string& member);

        bool sismember(
            const std::string& key,
            const std::string& member);
};