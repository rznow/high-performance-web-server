#ifndef COMMENT_H
#define COMMENT_H
#include <vector>
#include "mysql/StatementRow.h"
inline std::string safeStr(char* s)
{
    return s ? s : "";
}

inline int safeInt(char* s, int def = -1)
{
    return s ? std::stoi(s) : def;
}
struct Comment
{
    int comment_id = 0;
    int post_id = 0;
    int user_id = 0;

    int parent_id = 0;      // 回复哪条评论，0 表示一级评论
    int reply_user_id = 0;  // 回复的用户

    std::string author;
    std::string reply_author;
    std::string avatar;

    std::string content;

    std::string create_time;

    std::vector<Comment*> children;

    Comment() = default;
    Comment(StatementRow& row)
    {
        int idx = 0;
        comment_id      = row.getInt(idx++);
        post_id         = row.getInt(idx++);          //帖子的id
        user_id         = row.getInt(idx++);          //评论用户id

        parent_id       = row.isNull(idx)?0:row.getInt(idx);       //父评论id
        idx++;
        reply_user_id   = row.isNull(idx)?0:row.getInt(idx);       //父评论用户id
        idx++;

        author          = row.getString(idx++);          //当前评论作者
        reply_author    = row.getString(idx++);          //父评论作者
        content         = row.getString(idx++);          //文本内容
        create_time     = row.getString(idx++);
    };
    
    Comment(MYSQL_ROW& row)
    {
        int idx = 0;
        comment_id      = safeInt(row[idx++]);
        post_id         = safeInt(row[idx++]);          //帖子的id
        user_id         = safeInt(row[idx++]);          //评论用户id

        parent_id       = safeInt(row[idx++], 0);       //父评论id
        reply_user_id   = safeInt(row[idx++], 0);       //父评论用户id

        author          = safeStr(row[idx++]);          //当前评论作者
        reply_author    = safeStr(row[idx++]);          //父评论作者
        content         = safeStr(row[idx++]);          //文本内容
        create_time     = safeStr(row[idx]);
    };

    void print() const
    {
        std::cout << "comment_id:\t" << comment_id << '\n';
        std::cout << "post_id:\t"    << post_id << '\n';
        std::cout << "user_id:\t"    << user_id << '\n';
        std::cout << "parent_id:\t"    << parent_id << '\n';
        std::cout << "reply_user_id:\t"    << reply_user_id << '\n';
        std::cout << "author:\t\t"   << author << '\n';
        std::cout << "content:\t"    << content << '\n';
        std::cout << "create_time:\t" << create_time << '\n';
        std::cout << std::endl;
    }
};
#endif