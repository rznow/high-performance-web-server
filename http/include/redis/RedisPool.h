#pragma once

#include <queue>
#include <condition_variable>
#include <mutex>
#include <memory>

class Redis;
class RedisPool
{
    private:
        std::mutex mtx;
        std::condition_variable cv;
        std::queue<Redis*> pool;
        size_t capcity;
        RedisPool(int _capcity = 5);
        ~RedisPool();
    public:
        RedisPool(const RedisPool&) = delete;
        RedisPool& operator=(const RedisPool&) = delete;

        static RedisPool& getInstance();
        std::shared_ptr<Redis> getConnection();
        void releaseConnection(Redis* redis);

};