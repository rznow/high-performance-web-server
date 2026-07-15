#ifndef COMMENT_H
#define COMMENT_H
#include <vector>
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

    std::string content;

    std::string create_time;

    std::vector<Comment*> children;

    Comment() = default;
    Comment(MYSQL_ROW row)
    {
        int idx = 0;
        comment_id = safeInt(row[idx++]);
        post_id = safeInt(row[idx++]);
        user_id = safeInt(row[idx++]);

        parent_id = safeInt(row[idx++], 0);
        reply_user_id = safeInt(row[idx++], -1);

        author = safeStr(row[idx++]);
        reply_author = safeStr(row[idx++]);
        content = safeStr(row[idx++]);
        create_time = safeStr(row[idx++]);
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