#pragma once

#include <queue>
#include <condition_variable>
#include <mutex>
#include <memory>

class Redis;
class Config;
class RedisPool
{
    private:
        std::mutex mtx;
        std::condition_variable cv;
        std::queue<Redis*> pool;

        std::string host;
        int port;
        size_t capcity;
        bool initialized = false;

        RedisPool() = default;
        ~RedisPool();
    public:
        RedisPool(const RedisPool&) = delete;
        RedisPool& operator=(const RedisPool&) = delete;
        bool init(const Config& cfg);
        static RedisPool& getInstance();
        std::shared_ptr<Redis> getConnection();
        void releaseConnection(Redis* redis);

};