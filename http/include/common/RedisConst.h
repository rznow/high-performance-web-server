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

namespace CommentField
{
    inline constexpr const char* ID            = "id";
    inline constexpr const char* POST_ID       = "post_id";
    inline constexpr const char* USER_ID       = "user_id";
    inline constexpr const char* PARENT_ID     = "parent_id";
    inline constexpr const char* REPLY_USER_ID = "reply_user_id";

    inline constexpr const char* AUTHOR        = "author";
    inline constexpr const char* REPLY_AUTHOR  = "reply_author";

    inline constexpr const char* CONTENT       = "content";
    inline constexpr const char* CREATE_TIME   = "create_time";
}

namespace RedisKey
{
    inline std::string post(int id)
    {
        return "post:" + std::to_string(id);
    }

    inline std::string postsPage(int page, int size)
    {
        return "posts:page:" + std::to_string(page) + ":size:" + std::to_string(size);
    }

    inline std::string postsPage()
    {
        return "posts:page:*";
    }

    inline std::string user(int id)
    {
        return "user:" + std::to_string(id);
    }

    inline int deuser(std::string key)
    {
        return std::stoi(key.substr(5));
    }

    inline std::string comment(int id)
    {
        return "comment:" + std::to_string(id);
    }

    inline int decomment(std::string key)
    {
        return std::stoi(key.substr(8));
    }

    inline std::string commentsPage(int id)
    {
        return "post:" + std::to_string(id) + ":root_comments";
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

    inline std::string dirtyPost()
    {
        return "dirty:post";
    }

    inline std::string hotPosts()
    {
        return "posts:hot";
    }


}