#ifndef POST_H
#define POST_H

#include <string>
#include <iostream>
#include <unordered_map>
#include <mysql/mysql.h>
#include "mysql/StatementRow.h"
struct Post
{
    int post_id;
    int user_id;
    std::string author;
    std::string title;
    std::string content;
    int like_count;
    int comment_count;
    int view_count;
    std::string create_time;

    Post():post_id(-1){};
    Post(const Post&p) = default;
    Post& operator=(const Post&p) = default;
    Post(StatementRow& row);

    Post(MYSQL_ROW& row);

    Post(std::unordered_map<std::string, std::string>& fields);

    void print();
};
#endif