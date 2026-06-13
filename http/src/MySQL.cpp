#include "MySQL.h"
#include <iostream>

MySQL::MySQL()
{
    // 初始化MySQL对象
    conn = mysql_init(NULL);

}

MySQL::~MySQL()
{
    // 关闭数据库连接
    mysql_close(conn);
    conn = nullptr;
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
    // 执行SQL查询
    if (mysql_query(conn, sql.c_str()))
    {
        
        return true;
    }
    return false;
}

std::string MySQL::loginSQL(const std::string& name, const std::string& password)
{
    std::string sql = "SELECT * FROM user_info WHERE user_name = " + name +';';

    if(!query(sql)) 
    {
        mysql_close(conn);
        return "";
    }
    MYSQL_RES *res = mysql_store_result(conn);

    // 处理查询结果
    MYSQL_ROW row;
    int num_fields = mysql_num_fields(res);
    while ((row = mysql_fetch_row(res))) {
        for (int i = 0; i < num_fields; i++) {
            std::cout << (row[i] ? row[i] : "NULL") << " ";
        }
        std::cout << std::endl;
    }

    return "";
}

void MySQL::registerSQL(const std::string& name, const std::string& password)
{
    std::string sql = "INSERT INTO user_info(user_name, password) values('" + name + "', '" + password + "');";
    std::cout<< sql <<std::endl;
    if(!query(sql)) 
    {
        mysql_close(conn);
        return;
    }

    

}

MYSQL* MySQL::get()
{
    return conn;
}

