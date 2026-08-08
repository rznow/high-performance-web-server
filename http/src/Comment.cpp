#include "common/Comment.h"
#include "common/RedisConst.h"
#include <iostream>

inline std::string safeStr(char* s)
{
    return s ? s : "";
}

inline int safeInt(char* s, int def = -1)
{
    return s ? std::stoi(s) : def;
}

inline int safeInt(std::string s, int def = -1)
{
    return !s.empty() ? std::stoi(s) : def;
}

Comment::Comment(StatementRow& row)
{
    int idx = 0;
    comment_id      = row.getInt(idx++);
    post_id         = row.getInt(idx++);          //帖子的id
    user_id         = row.getInt(idx++);          //评论用户id

    parent_id       = row.isNull(idx)?0:row.getInt(idx);       //父评论id
    idx++;
    root_comment_id  = row.isNull(idx)?0:row.getInt(idx);       //根评论id
    idx++;
    reply_user_id   = row.isNull(idx)?0:row.getInt(idx);       //父评论用户id
    idx++;
    
    author          = row.getString(idx++);          //当前评论作者
    reply_author    = row.getString(idx++);          //父评论作者
    avatar          = row.getString(idx++);          //头像
    content         = row.getString(idx++);          //文本内容
    create_time     = row.getString(idx++);
};

Comment::Comment(MYSQL_ROW& row)
{
    int idx = 0;
    comment_id      = safeInt(row[idx++]);
    post_id         = safeInt(row[idx++]);          //帖子的id
    user_id         = safeInt(row[idx++]);          //评论用户id

    parent_id       = safeInt(row[idx++], 0);       //父评论id
    root_comment_id  = safeInt(row[idx++], 0);          //根评论id
    reply_user_id   = safeInt(row[idx++], 0);       //父评论用户id
    
    author          = safeStr(row[idx++]);          //当前评论作者
    reply_author    = safeStr(row[idx++]);          //父评论作者
    avatar          = safeStr(row[idx++]);                //头像
    content         = safeStr(row[idx++]);          //文本内容
    create_time     = safeStr(row[idx]);
};

Comment::Comment(std::unordered_map<std::string, std::string>& fields)
{
    auto get = [&](const std::string& key) -> std::string
    {
        auto it = fields.find(key);

        if (it == fields.end())
            return {};

        return it->second;
    };


    comment_id     = safeInt(get(CommentField::ID));
    post_id        = safeInt(get(CommentField::POST_ID));
    user_id        = safeInt(get(CommentField::USER_ID));

    parent_id      = safeInt(get(CommentField::PARENT_ID));
    root_comment_id = safeInt(get(CommentField::ROOT_COMMENT_ID));
    reply_user_id  = safeInt(get(CommentField::REPLY_USER_ID));

    author         = get(CommentField::AUTHOR);
    reply_author   = get(CommentField::REPLY_AUTHOR);
    avatar         = get(CommentField::AVATAR);

    content        = get(CommentField::CONTENT);
    create_time    = get(CommentField::CREATE_TIME);

};

void Comment::print() const
{
    std::cout << "comment_id:\t" << comment_id << '\n';
    std::cout << "post_id:\t"    << post_id << '\n';
    std::cout << "user_id:\t"    << user_id << '\n';
    std::cout << "parent_id:\t"    << parent_id << '\n';
    std::cout << "root_comment_id:\t"    << root_comment_id << '\n';
    std::cout << "reply_user_id:\t"    << reply_user_id << '\n';
    std::cout << "author:\t\t"   << author << '\n';
    std::cout << "reply_author:\t\t"   << reply_author << '\n';
    std::cout << "avatar:\t\t"   << avatar << '\n';
    std::cout << "content:\t"    << content << '\n';
    std::cout << "create_time:\t" << create_time << '\n';
    std::cout << std::endl;
}