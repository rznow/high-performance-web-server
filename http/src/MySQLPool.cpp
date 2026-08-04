#include "mysql/MySQLPool.h"
#include "mysql/MySQL.h"
#include "util/Config.h"

// MySQLPool::MySQLPool()
// {
    // for(size_t i=0;i < _capcity;i++)
    // {
    //     MySQL* mysql = new MySQL();
    //     if (!mysql->connect())
    //     {
    //         delete mysql;
    //         continue;
    //     }
    //     // mysql->query(R"(select * from user_info;)");
    //     pool.push(mysql);
    // }
// }

MySQLPool::~MySQLPool()
{
    std::lock_guard<std::mutex> lock(mtx);
    while (!pool.empty()) {
        delete pool.front();
        pool.pop();
    }
}

bool MySQLPool::init(const Config& cfg)
{
    if(initialized)
        return true;

    host = cfg.get("mysql_host", "127.0.0.1");
    user = cfg.get("mysql_user", "webserver");
    password = cfg.get("mysql_password", "123456");
    database = cfg.get("mysql_database", "miniforum");
    port = cfg.getInt("mysql_port", 3306);

    capcity = cfg.getInt("mysql_pool_size", 10);

    for(size_t i = 0; i < capcity; i++)
    {
        auto conn = new MySQL();

        if(!conn->connect(
                host,
                user,
                password,
                database,
                port))
        {
            return false;
        }

        pool.push(std::move(conn));
    }

    initialized = true;
    return true;
}

void MySQLPool::createConns()
{
    for(size_t i=0;i<3;i++)
    {
        MySQL* mysql = new MySQL();

        if (!mysql->connect(
                host,
                user,
                password,
                database,
                port))
        {
            delete mysql;
            continue;
        }

        pool.push(std::move(mysql));
    }
}

MySQLPool& MySQLPool::getInstance()
{
    static MySQLPool pool;
    return pool;
}

std::shared_ptr<MySQL> MySQLPool::getConnection()
{
    MySQL* mysql;
    
    {
        std::unique_lock<std::mutex> ul(mtx);

        cv.wait(ul, [&]{ return !pool.empty(); });

        if(pool.size() < capcity)
        {
            createConns();
        }

        mysql = pool.front();
        pool.pop();
    }

    
    if(mysql_ping(mysql->get()) != 0)
    {
        mysql->reconnect(
                    host,
                    user,
                    password,
                    database,
                    port);
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
            mysql->reconnect(
                        host,
                        user,
                        password,
                        database,
                        port);
        }
        pool.push(mysql);
    }
    
    cv.notify_one();
}