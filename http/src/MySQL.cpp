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
    if (mysql_query(conn, sql.c_str())==0)
    {
        
        return true;
    }
    return false;
}

bool MySQL::loginSQL(const std::string& name, const std::string& password)
{
    std::string sql = "SELECT password FROM user_info WHERE user_name = '" + name +"';";

    query(sql);

    MYSQL_RES *res = mysql_store_result(conn);

    // 处理查询结果
    MYSQL_ROW row = mysql_fetch_row(res);
    int num_fields = mysql_num_fields(res); //1

    if(row[0] != password) 
    {
        std::cout<<" wrong password !"<<std::endl;
        return false;
    }
    std::cout<<" successful login !"<<std::endl;
    return true;
}

bool MySQL::registerSQL(const std::string& name, const std::string& password)
{
    std::string sql = "INSERT INTO user_info(user_name, password) values('" + name + "', '" + password + "');";
    std::cout<< sql <<std::endl;
    if(!query(sql)) 
    {
        std::cout<<" register failed !"<<std::endl;
        return false;
    }
    std::cout<<" successful register !"<<std::endl;
    return true;
}

MYSQL* MySQL::get()
{
    return conn;
}

