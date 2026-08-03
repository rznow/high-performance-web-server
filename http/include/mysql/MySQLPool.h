#ifndef MYSQLPOOL_H
#define MYSQLPOOL_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <memory>

class MySQL;
class Config;
class MySQLPool {
private:
    std::queue<MySQL*> pool;
    std::mutex mtx;
    std::condition_variable cv;
    size_t capcity;
    bool initialized = false;
    MySQLPool() = default;
    ~MySQLPool();
    void createConns();

    std::string host;
    std::string user;
    std::string password;
    std::string database;
    int port;
public:
    MySQLPool(const MySQLPool&) = delete;
    MySQLPool& operator=(const MySQLPool&) = delete;
    bool init(const Config& cfg);
    static MySQLPool& getInstance();
    std::shared_ptr<MySQL> getConnection();
    void releaseConnection(MySQL* mysql);
};





#endif