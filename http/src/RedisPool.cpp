#include "redis/RedisPool.h"
#include "redis/Redis.h"
#include "util/Config.h"


// RedisPool::RedisPool()
// {
//     for(int i=0;i < _capcity;i++)
//     {
//         Redis* redis = new Redis();
//         if(!redis->connect())
//         {
//             delete redis;
//             continue;
//         }
//         pool.push(redis);
//     }
// }

RedisPool::~RedisPool()
{
    std::unique_lock<std::mutex> ul(mtx);
    while(!pool.empty())
    {
        delete pool.front();
        pool.pop();
    }
}

bool RedisPool::init(const Config& cfg)
{
    if(initialized)
        return true;

    host = cfg.get("redis_host", "127.0.0.1");
    port = cfg.getInt("redis_port", 6379);
    capcity = cfg.getInt("redis_pool_size", 10);

    for(size_t i = 0; i < capcity; i++)
    {
        auto conn = new Redis();

        if(!conn->connect(
                host,
                port))
        {
            return false;
        }
        pool.push(std::move(conn));
    }

    initialized = true;
    return true;
}

RedisPool& RedisPool::getInstance()
{
    static RedisPool redisPool;
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
                if(!redis->connect(
                        host,
                        port))
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
        redis->connect(host, port);
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
    if(!redis->valid())  
    {
        redis->connect(host, port);
    }
    pool.push(redis);



    cv.notify_one();
}