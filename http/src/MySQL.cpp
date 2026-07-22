#include "mysql/MySQL.h"
#include "common/UserInfo.h"
#include <mysql/mysql.h>
#include <common/Post.h>
#include <common/Comment.h>
#include <iostream>
#include <mysql/Statement.h>

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
    Statement stmt(conn,
        "SELECT user_id,password "
        "FROM user_info "
        "WHERE user_name=?");

    stmt.bindString(0,name);

    if(!stmt.execute())
        return -1;

    stmt.storeResult();

    int id;
    char dbPassword[128];

    stmt.bindResultInt(0,id);
    stmt.bindResultString(1,dbPassword,sizeof(dbPassword));

    if(!stmt.fetch())
        return -1;

    if(password != dbPassword)
        return 0;

    user.user_id=id;
    user.user_name=name;
    
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
    std::string sql = R"(
    INSERT INTO 
    posts(
        user_id, 
        title, 
        content)
    VALUES( )" 
    + std::to_string(p.user_id) + " , '" 
    + p.title + "' , '" 
    + p.content 
    + "' );";
    
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

void MySQL::saveComment(Comment& c)
{
    std::string sql = R"(
    INSERT INTO comments(
        post_id,
        user_id,
        parent_id,
        reply_user_id,
        content
    )
    VALUES(
    )"
    + std::to_string(c.post_id)
    + ","
    + std::to_string(c.user_id)
    + ","
    + std::to_string(c.parent_id)
    + ","
    + (c.reply_user_id==-1?"NULL":std::to_string(c.reply_user_id))
    + ",'"
    + c.content
    + "');";

    query(sql);
    int comment_id = mysql_insert_id(conn);
    sql =
    "UPDATE posts "
    "SET comment_count = comment_count + 1 "
    "WHERE post_id = "
    + std::to_string(c.post_id)
    + ";";


    query(sql);

    sql = R"(
    SELECT
        c.comment_id,
        c.post_id,
        c.user_id,

        c.parent_id,
        c.reply_user_id,

        u.user_name,
        u2.user_name AS reply_name,
        c.content,
        c.create_time
    FROM comments c
    JOIN user_info u
    ON c.user_id=u.user_id

    LEFT JOIN user_info u2
    ON c.reply_user_id=u2.user_id

    WHERE c.comment_id=)" + std::to_string(comment_id) +
    " ; ";

    query(sql);
    MYSQL_RES* res = mysql_store_result(conn);
    if(!res) return;
    MYSQL_ROW row = mysql_fetch_row(res);
    if(!row) return;
    c = Comment(row);
    c.print();
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
        p.like_count,
        p.comment_count,
        p.view_count,
        p.create_time
    FROM posts p
    INNER JOIN user_info u
    ON p.user_id = u.user_id
    WHERE p.deleted = 0
    ORDER BY p.create_time DESC
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
        Post p(row);
        posts.push_back(std::move(p));
    }
    
}

void MySQL::getRootComments(std::vector<Comment>& comments, size_t post_id, size_t size, size_t offset)
{
    std::string sql = R"(
    SELECT
        c.comment_id,
        c.post_id,
        c.user_id,

        c.parent_id,
        c.reply_user_id,

        u.user_name,
        u2.user_name AS reply_name,
        c.content,
        c.create_time
    FROM comments c
    JOIN user_info u
    ON c.user_id=u.user_id

    LEFT JOIN user_info u2
    ON c.reply_user_id=u2.user_id

    WHERE c.post_id=)" + std::to_string(post_id) +
    R"(
    AND parent_id = 0
    ORDER BY
    parent_id,
    create_time
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
        comments.emplace_back(row);
    }
}

void MySQL::getComments(std::vector<Comment>& comments, size_t post_id)
{
    std::string sql = R"(
    SELECT
        c.comment_id,
        c.post_id,
        c.user_id,

        c.parent_id,
        c.reply_user_id,

        u.user_name,
        u2.user_name AS reply_name,
        c.content,
        c.create_time
    FROM comments c
    JOIN user_info u
    ON c.user_id=u.user_id

    LEFT JOIN user_info u2
    ON c.reply_user_id=u2.user_id

    WHERE c.post_id=)" + std::to_string(post_id) +
    R"(
    ORDER BY
    parent_id,
    create_time;)";
    

    if(!query(sql)) return;
    MYSQL_RES * res = mysql_store_result(conn);

    if(res == nullptr) return;

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res)) != nullptr)
    {
        comments.emplace_back(row);
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
        p.like_count,
        p.comment_count,
        p.view_count,
        p.create_time
    FROM posts p
    INNER JOIN user_info u
    ON p.user_id = u.user_id
    where p.post_id = )" 
    + std::to_string(post_id) +
    " and p.deleted <> 1"+
    ";";

    if(!query(sql)) return false;
    MYSQL_RES * res = mysql_store_result(conn);

    if(res == nullptr) return false;

    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    
    p = Post(row);
   
    p.print();

    return true;
}

bool MySQL::delPost(int post_id)
{
    std::string sql = R"(
    UPDATE
    posts
    SET deleted = 1
    where post_id = )" 
    + std::to_string(post_id) +
    ";";
    std::cout<<sql<<std::endl;
    if(!query(sql)) return false;

    return true;
}

int MySQL::like(int post_id, int user_id, bool& liked)
{
    std::string sql = R"(
    SELECT * FROM
    post_like
    where post_id = )" 
    + std::to_string(post_id) +
    " and user_id = "
    + std::to_string(user_id) +
    ";";

    if(!query(sql)) return -1;
    MYSQL_RES* res = mysql_store_result(conn);
    if(res == nullptr) return -1;
    MYSQL_ROW row = mysql_fetch_row(res);

    if(row == nullptr) //没有点赞
    {
        sql = R"(
            INSERT INTO post_like
            (user_id, post_id)
            VALUES( )" + 
            std::to_string(user_id) +
            " , " + 
            std::to_string(post_id) + 
            ");";
        if(!query(sql)) return -1;
        sql = R"(
            UPDATE posts
            SET like_count =
            like_count + 1
            WHERE post_id = )" + 
            std::to_string(post_id) + 
            ";";
        if(!query(sql)) return -1;
        liked = true;
    }else
    {
        sql = R"(
            DELETE FROM post_like
            WHERE user_id = )" +
            std::to_string(user_id) +
            " and post_id = " + 
            std::to_string(post_id) + 
            ";";
        if(!query(sql)) return -1;
        sql = R"(
            UPDATE posts
            SET like_count =
            like_count - 1
            WHERE post_id = )" + 
            std::to_string(post_id) + 
            ";";
        if(!query(sql)) return -1;
        liked = false;
    }

    sql = R"(
        SELECT like_count 
        FROM posts
        WHERE post_id = )" + 
        std::to_string(post_id) + 
        ";";
    if(!query(sql)) return -1;

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);

    return std::stoi(row[0]);
}

bool MySQL::liked(int post_id, int user_id)
{
    std::string sql = R"(
    SELECT * FROM
    post_like
    where post_id = )" 
    + std::to_string(post_id) +
    " and user_id = "
    + std::to_string(user_id) +
    ";";

    if(!query(sql)) return false;
    MYSQL_RES* res = mysql_store_result(conn);
    if(res == nullptr) return false;
    MYSQL_ROW row = mysql_fetch_row(res);

    if(row == nullptr) return false;
    return true;
}

int MySQL::view(int post_id)
{
    std::string sql = R"(
    UPDATE posts
    SET view_count =
    view_count + 1
    WHERE post_id = )" + 
    std::to_string(post_id) + 
    ";";
    if(!query(sql)) return 0;
    MYSQL_RES* res = mysql_store_result(conn);
    if(res == nullptr) return 0;
    MYSQL_ROW row = mysql_fetch_row(res);
    return row != nullptr?std::stoi(row[0]):0;
}

bool MySQL::checkPost(int post_id, int user_id)
{
    std::string sql = R"(
    SELECT * FROM
    posts
    where post_id = )" 
    + std::to_string(post_id) +
    " and user_id = "
    + std::to_string(user_id) +
    ";";

    if(!query(sql)) return false;
    MYSQL_RES* res = mysql_store_result(conn);
    if(res == nullptr) return false;
    MYSQL_ROW row = mysql_fetch_row(res);

    if(row == nullptr) return false;
    return true;
}

bool MySQL::modPost(int post_id, std::string title, std::string content)
{
    std::string sql = R"(
    UPDATE posts
    SET title = ')" + 
    title + "',"
    " content = '" + 
    content + "'"
    "where post_id = " +
    std::to_string(post_id) +
    ";";

    if(!query(sql)) return false;
    return true;
}