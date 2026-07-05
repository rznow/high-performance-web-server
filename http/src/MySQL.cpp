#include "mysql/MySQL.h"
#include <mysql/mysql.h>
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
    int ret = mysql_query(conn, sql.c_str());

    if(ret != 0)
    {
        std::cout << "SQL failed: " << sql << std::endl;
        std::cout << "Error: " << mysql_error(conn) << std::endl;
        return false;
    }
    std::cout << "SQL success: " << sql << std::endl;
    return true;
}

bool MySQL::reconnect()
{
    // 先释放旧连接
    if(conn != nullptr)
    {
        mysql_close(conn);
        conn = nullptr;
    }

    // 重新初始化
    conn = mysql_init(nullptr);
    if(conn == nullptr)
    {
        std::cout << "mysql_init failed\n";
        return false;
    }

    connect();

    if(conn == nullptr)
    {
        std::cout << "mysql_real_connect failed: "
                  << mysql_error(conn) << std::endl;
        return false;
    }

    return true;
}
/*
    *未注册返回-1
    *密码错误返回0
    *登录成功返回1
*/
int MySQL::loginSQL(const std::string& name, const std::string& password)
{
    std::string sql = "SELECT password FROM user_info WHERE user_name = '" + name +"';";

    if(!query(sql)) return -1;

    MYSQL_RES *res = mysql_store_result(conn);

    if(!res)
    {
        return -1;
    }

    // 处理查询结果
    MYSQL_ROW row = mysql_fetch_row(res);
    // int num_fields = mysql_num_fields(res); //1
    
    if(row == nullptr)
    {
        mysql_free_result(res);
        return -1; // 用户不存在
    }

    if(row[0] != password) 
    {
        std::cout<<" wrong password !"<<std::endl;
        return 0;
    }

    std::cout<<" successful login !"<<std::endl;
    mysql_free_result(res);
    return 1;
}
/*
    *异常返回-1
    *已注册返回0
    *注册成功返回1
*/
int MySQL::registerSQL(const std::string& name, const std::string& password)
{
    std::string check_sql =
        "SELECT user_id FROM user_info WHERE user_name='" + name + "';";
    
    if(!query(check_sql))   return -1;

    MYSQL_RES* res = mysql_store_result(conn);
    
    if(!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);

    // 用户已存在
    if(row != nullptr)
    {
        mysql_free_result(res);
        std::cout << "user already exists!" << std::endl;
        return 0;
    }

    mysql_free_result(res);

    std::string insert_sql = "INSERT INTO user_info(user_name, password) VALUES('" + name + "','" + password + "');";
    std::cout<<insert_sql<<std::endl;
    if(!query(insert_sql))
    {
        std::cout << "register failed!" << std::endl;
        return -1;
    }

    std::cout << "register success!" << std::endl;
    return 1;
}

MYSQL* MySQL::get()
{
    return conn;
}

