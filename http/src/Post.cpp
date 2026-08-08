#include "common/Post.h"
#include "common/RedisConst.h"
inline int safeInt(char* s, int def = -1)
{
    return s ? std::stoi(s) : def;
}

inline int safeInt(std::string s, int def = -1)
{
    return !s.empty() ? std::stoi(s) : def;
}

Post::Post(StatementRow& row)
{
    int idx = 0;
    post_id       = row.getInt(idx++);
    user_id       = row.getInt(idx++);      
    author        = row.getString(idx++);
    title         = row.getString(idx++);
    content       = row.getString(idx++);
    like_count    = row.getInt(idx++);
    comment_count = row.getInt(idx++);
    view_count    = row.getInt(idx++);
    create_time   = row.getString(idx++);
};

Post::Post(MYSQL_ROW& row)
{
    int idx = 0;
    post_id       = safeInt(row[idx++]);
    user_id       = safeInt(row[idx++]);      
    author        = row[idx++];
    title         = row[idx++];
    content       = row[idx++];
    like_count    = safeInt(row[idx++]);
    comment_count = safeInt(row[idx++]);
    view_count    = safeInt(row[idx++]);
    create_time   = row[idx++];
};

Post::Post(std::unordered_map<std::string, std::string>& fields)
{
    auto get = [&](const std::string& key) -> std::string
    {
        auto it = fields.find(key);

        if (it == fields.end())
            return {};

        return it->second;
    };

    post_id       = safeInt(get(PostField::ID));
    user_id       = safeInt(get(PostField::USER_ID));

    author        = get(PostField::AUTHOR);
    title         = get(PostField::TITLE);
    content       = get(PostField::CONTENT);

    like_count    = safeInt(get(PostField::LIKE));
    comment_count = safeInt(get(PostField::COMMENT));
    view_count    = safeInt(get(PostField::VIEW));

    create_time   = get(PostField::TIME);
}

void Post::print()
{
    std::cout<<"post_id:\t"<<post_id<<std::endl;
    std::cout<<"user_id:\t"<<user_id<<std::endl;
    std::cout<<"author:\t\t"<<author<<std::endl;
    std::cout<<"title:\t\t"<<title<<std::endl;
    std::cout<<"content:\t"<<content<<std::endl;
    std::cout<<"like_count:\t"<<like_count<<std::endl;
    std::cout<<"comment_count:\t"<<comment_count<<std::endl;
    std::cout<<"view_count:\t"<<view_count<<std::endl;
    std::cout<<"create_time:\t"<<create_time<<std::endl;
    std::cout<<std::endl;
}