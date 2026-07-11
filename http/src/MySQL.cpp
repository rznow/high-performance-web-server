#include "mysql/MySQL.h"
#include "common/UserInfo.h"
#include <mysql/mysql.h>
#include <common/Post.h>
#include <common/PostCache.h>
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
    std::cout << "SQL success: " << sql << std::endl << std::endl;
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
int MySQL::loginSQL(const std::string& name, const std::string& password, UserInfo& user)
{
    std::string sql = "SELECT user_id,password FROM user_info WHERE user_name = '" + name +"';";

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

    if(row[1] != password) 
    {
        std::cout<<" wrong password !"<<std::endl;
        return 0;
    }

    user.user_id = std::stoi(row[0]);
    user.user_name = name;

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

int MySQL::savePost(Post& p)
{
    std::string sql = "INSERT INTO posts(user_id, title, content) \
                       VALUES('" + std::to_string(p.user_id) + "','" + p.title + "','" + p.content + "');";
    
    query(sql);

    int post_id = mysql_insert_id(conn);
    p.post_id = post_id;

    //从mysql中获取表项(帖子)创建时间
    std::string create_time_sql = "SELECT create_time FROM posts WHERE post_id=" + std::to_string(post_id);
    query(create_time_sql);
    MYSQL_RES* res = mysql_store_result(conn);
    // size_t t = mysql_num_fields(res);
    MYSQL_ROW row = mysql_fetch_row(res);
    // std::cout<<"mysql_num_fields:"<<t<<std::endl;
    p.create_time = row[0];
    return post_id;
}

void MySQL::getPosts(std::vector<Post>& posts,size_t size,size_t offset)
{
    std::string sql = R"(
    SELECT
        p.post_id,
        p.user_id,
        u.user_name,
        p.title,
        p.content,
        p.create_time
    FROM posts p
    INNER JOIN user_info u
    ON p.user_id = u.user_id
    ORDER BY p.create_time ASC
    LIMIT )" 
    + std::to_string(size) +
    " OFFSET " +
    std::to_string(offset) +
    ";";
    

    if(!query(sql)) return;
    MYSQL_RES * res = mysql_store_result(conn);

    if(res == nullptr) return;

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res)) != nullptr)
    {
        Post p;
        p.post_id = std::stoi(row[0]);
        p.user_id = std::stoi(row[1]);
        p.author = row[2];
        p.title = row[3];
        p.content = row[4];
        p.create_time = row[5];

        posts.push_back(std::move(p));
    }
    
    
}

bool MySQL::getPost(int post_id, Post& p)
{
    std::string sql = R"(
    SELECT
        p.post_id,
        p.user_id,
        u.user_name,
        p.title,
        p.content,
        p.create_time
    FROM posts p
    INNER JOIN user_info u
    ON p.user_id = u.user_id
    where p.post_id = )" 
    + std::to_string(post_id) +
    ";";

    if(!query(sql)) return false;
    MYSQL_RES * res = mysql_store_result(conn);

    if(res == nullptr) return false;

    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    
    p.post_id = std::stoi(row[0]);
    p.user_id = std::stoi(row[1]);
    p.author = row[2];
    p.title = row[3];
    p.content = row[4];
    p.create_time = row[5];


    return true;
}

bool MySQL::delPost(int post_id)
{
    std::string sql = R"(
    DELETE
    FROM posts
    where post_id = )" 
    + std::to_string(post_id) +
    ";";
    std::cout<<sql<<std::endl;
    if(!query(sql)) return false;

    return true;
}
