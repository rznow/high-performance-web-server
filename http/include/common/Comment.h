#ifndef COMMENT_H
#define COMMENT_H
#include <vector>
#include <unordered_map>
#include <mysql/mysql.h>
#include "mysql/StatementRow.h"

struct Comment
{
    int comment_id = 0;
    int post_id = 0;
    int user_id = 0;

    int parent_id = 0;      // 回复哪条评论，0 表示一级评论
    int root_comment_id;     // 所属根评论
    int reply_user_id = 0;  // 回复的用户
    

    std::string author;
    std::string reply_author;
    std::string avatar;

    std::string content;

    std::string create_time;

    std::vector<Comment*> children;

    Comment() = default;
    Comment(StatementRow& row);
    
    Comment(MYSQL_ROW& row);

    Comment(std::unordered_map<std::string, std::string>& fields);

    void print() const;
};
#endif