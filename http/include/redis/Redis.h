#pragma once

#include <hiredis/hiredis.h>
#include <string>
#include <unordered_map>
#include "redis/RedisValue.h"

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

class Redis
{
    private:
        redisContext *c;
    public:
        Redis() = default;
        ~Redis();

        bool connect(
            const std::string& host,
            int port);

        bool valid();

        bool set(const std::string& key, const std::string& value);
        RedisValue get(const std::string& key);
        bool exists(const std::string& key);
        bool del(const std::string& key);
        void delByPattern(const std::string& pattern);
        bool expire(const std::string& key, int seconds);

        RedisValue incr(const std::string& key);

        bool lrange(
            const std::string& key, 
            std::vector<std::string>& values,
            int start = 0,
            int end = -1);

        bool lpush(
            const std::string& key, 
            const std::vector<std::string>& values);

        bool rpush(
            const std::string& key, 
            const std::vector<std::string>& values);

        bool hmset(
            const std::string& key, 
            const std::unordered_map<std::string,std::string>& fields);

        bool hmget(
            const std::string& key, 
            std::unordered_map<std::string,std::string>& fields);

        bool hgetAll(
            const std::string& key, 
            std::unordered_map<std::string,std::string>& fields);

        RedisValue hget(
            const std::string& key, 
            const std::string& field);

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

        bool smembers(
            const std::string& key,
            std::vector<std::string>& values);

        bool spop(
            const std::string& key,
            std::string& value);

        bool zadd(
            const std::string& key,
            const std::vector<std::pair<double,std::string>>& values);

        bool zrange(
            const std::string& key, 
            std::vector<std::string>& values,
            int start = 0,
            int end = -1);

        bool zcard(
            const std::string& key,
            size_t& count);

        bool zcount(
            const std::string& key,
            const std::string& min,
            const std::string& max,
            size_t& count);

        bool zremrangebyscore(
            const std::string& key,
            const std::string& min,
            const std::string& max);

        bool pipeline(
            std::vector<std::string>& cmds, 
            std::vector<std::unordered_map<std::string, std::string>>& values);
};