#ifndef MYSQLPOOL_H
#define MYSQLPOOL_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <memory>

class MySQL;
class MySQLPool {
private:
    std::queue<MySQL*> pool;
    std::mutex mtx;
    std::condition_variable cv;
    int capcity;
    MySQLPool(size_t _capcity = 5);
    ~MySQLPool();
public:
    MySQLPool(const MySQLPool&) = delete;
    MySQLPool& operator=(const MySQLPool&) = delete;
    static MySQLPool& getInstance();
    std::shared_ptr<MySQL> getConnection();
    void releaseConnection(MySQL* mysql);
};





#endif