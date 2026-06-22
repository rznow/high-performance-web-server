#include "MySQLPool.h"
#include "MySQL.h"

MySQLPool::MySQLPool(size_t _capcity):capcity(_capcity)
{
    for(size_t i=0;i < _capcity;i++)
    {
        MySQL* mysql = new MySQL();
        if (!mysql->connect())
        {
            delete mysql;
            continue;
        }
        // mysql->query(R"(select * from user_info;)");
        pool.push(mysql);
    }
}
MySQLPool::~MySQLPool()
{
    std::lock_guard<std::mutex> lock(mtx);
    while (!pool.empty()) {
        delete pool.front();
        pool.pop();
    }
}

MySQLPool& MySQLPool::getInstance()
{
    static MySQLPool pool(10);
    return pool;
}

std::shared_ptr<MySQL> MySQLPool::getConnection()
{
    MySQL* mysql;
    
    {
        std::unique_lock<std::mutex> ul(mtx);

        cv.wait(ul, [&]{ return !pool.empty(); });

        mysql = pool.front();
        pool.pop();
    }

    
    if(mysql_ping(mysql->get()) != 0)
    {
        mysql->reconnect();
    }
    return std::shared_ptr<MySQL>(mysql, [this](MySQL* p)
        {
            releaseConnection(p);
        }   
    );
}

void MySQLPool::releaseConnection(MySQL* mysql)
{
    {
        std::unique_lock<std::mutex> ul(mtx);
        if(mysql_ping(mysql->get()) != 0)
        {
            mysql->reconnect();
        }
        pool.push(mysql);
    }
    
    cv.notify_one();
}