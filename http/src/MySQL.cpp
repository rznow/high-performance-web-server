#include "MySQL.h"


MySQL::MySQL()
{
    // 初始化MySQL对象
    MYSQL *conn = mysql_init(NULL);

}

bool MySQL::connect(const std::string& host,
        const std::string& user,
        const std::string& password,
        const std::string& db,
        int port)
{
    //mysql_real_connect 用于链接数据库
    return mysql_real_connect(
               conn,
               host.c_str(),
               user.c_str(),
               password.c_str(),
               db.c_str(),
               port,
               nullptr,
               0
           ) != nullptr;
}


bool MySQL::query(const std::string& sql)
{
    
}

MYSQL_RES* MySQL::storeResult()
{
    
}

MYSQL* MySQL::get()
{
    return conn;
}

