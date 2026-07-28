#include "redis/RedisPool.h"
#include "redis/Redis.h"


RedisPool::RedisPool(int _capcity):capcity(_capcity)
{
    for(int i=0;i < _capcity;i++)
    {
        Redis* redis = new Redis();
        if(!redis->connect())
        {
            delete redis;
            continue;
        }
        pool.push(redis);
    }
}

RedisPool::~RedisPool()
{
    std::unique_lock<std::mutex> ul(mtx);
    while(!pool.empty())
    {
        delete pool.front();
        pool.pop();
    }
}

RedisPool& RedisPool::getInstance()
{
    static RedisPool redisPool(10);
    return redisPool;
}

std::shared_ptr<Redis> RedisPool::getConnection()
{
    Redis* redis;
    
    {
        std::unique_lock<std::mutex> ul(mtx);

        cv.wait(ul, [&]{ return !pool.empty(); });

        if(pool.size() < capcity)
        {
            for(int i=0;i < 3;i++)
            {
                Redis* redis = new Redis();
                if(!redis->connect())
                {
                    delete redis;
                    continue;
                }
                pool.push(redis);
            }
        }

        redis = pool.front();
        pool.pop();
    }

    if(!redis->valid())
    {
        redis->connect();
    }

    return std::shared_ptr<Redis>(redis, [this](Redis* r)
            {
                releaseConnection(r);
            }
        ); 
}

void RedisPool::releaseConnection(Redis* redis)
{
    std::unique_lock<std::mutex> ul(mtx);
    if(redis->connect())
    {
        pool.push(redis);
    }else   delete redis;

    cv.notify_one();
}