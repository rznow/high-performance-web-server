namespace PostField
{
    inline constexpr const char* ID      = "id";
    inline constexpr const char* TITLE   = "title";
    inline constexpr const char* AUTHOR  = "author";
    inline constexpr const char* CONTENT = "content";
    inline constexpr const char* USER_ID = "user_id";
    inline constexpr const char* LIKE    = "like";
    inline constexpr const char* COMMENT = "comment";
    inline constexpr const char* VIEW    = "view";
    inline constexpr const char* TIME    = "time";
}

namespace RedisKey
{
    inline std::string post(int id)
    {
        return "post:" + std::to_string(id);
    }

    inline std::string user(int id)
    {
        return "user:" + std::to_string(id);
    }

    inline std::string comment(int id)
    {
        return "comment:" + std::to_string(id);
    }

    inline std::string postComments(int id)
    {
        return "post:" + std::to_string(id) + ":comments";
    }

    inline std::string postLikes(int id)
    {
        return "post:" + std::to_string(id) + ":likes";
    }

    inline std::string userLikes(int id)
    {
        return "user:" + std::to_string(id) + ":likes";
    }

    inline std::string latestPosts()
    {
        return "posts:latest";
    }

    inline std::string hotPosts()
    {
        return "posts:hot";
    }
}