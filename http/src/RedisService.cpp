#include "service/RedisService.h"
#include "common/RedisConst.h"
#include "redis/Redis.h"
#include <unordered_map>

using namespace PostField;
using namespace CommentField;
using namespace RedisKey;

constexpr std::string INCR = "1";
constexpr std::string DECR = "-1";

time_t StringToDatetime(std::string str)
{
    char *cha = (char*)str.data();             // 将string转换成char*。
    tm tm_;                                    // 定义tm结构体。
    int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
    sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);// 将string存储的日期时间，转换为int临时变量。
    tm_.tm_year = year - 1900;                 // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
    tm_.tm_mon = month - 1;                    // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
    tm_.tm_mday = day;                         // 日。
    tm_.tm_hour = hour;                        // 时。
    tm_.tm_min = minute;                       // 分。
    tm_.tm_sec = second;                       // 秒。
    tm_.tm_isdst = 0;                          // 非夏令时。
    time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
    return t_;                                 // 返回值。
}

RedisService::RedisService():pool(RedisPool::getInstance()){}

RedisService::~RedisService(){}

RedisService& RedisService::getInstance()
{
    static RedisService redisService;
    return redisService;
}

bool RedisService::setUser(UserInfo& u) const
{
    auto redis = pool.getConnection();

    if(!redis->exists(RedisKey::user(u.user_id)))
    {
        redis->hmset(RedisKey::user(u.user_id),{
        {UserField::ID,             std::to_string(u.user_id)},
        {UserField::NAME,           u.user_name},
        {UserField::AVATAR,         u.avatar}
        });
    }

    redis->expire(RedisKey::user(u.user_id), 1800);

    return true;
}

bool RedisService::expireUser(UserInfo& u) const
{
    auto redis = pool.getConnection();

    if(redis->exists(RedisKey::user(u.user_id)))
    {
        redis->expire(RedisKey::user(u.user_id), 1800);
    }

    return true;
}



bool RedisService::delUser(int user_id) const
{
    auto redis = pool.getConnection();

    return redis->del(RedisKey::user(user_id));
}

bool RedisService::setCreateTime(int user_id, const std::string& time) const
{
    auto redis = pool.getConnection();

    bool res = redis->set(RedisKey::userCreateTime(user_id), time);

    redis->expire(RedisKey::userCreateTime(user_id), 1800);

    return res;
}


bool RedisService::getCreateTime(int user_id, std::string& time) const
{
    auto redis = pool.getConnection();

    RedisValue rv = redis->get(RedisKey::userCreateTime(user_id));

    if(!rv.isString()) return false;

    time = rv.asString();
    return true;
}

bool RedisService::getAvatar(int user_id, std::string& avatar) const
{
    auto redis = pool.getConnection();

    RedisValue rv = redis->hget(RedisKey::user(user_id), UserField::AVATAR);

    if(!rv.isString()) return false;

    avatar = rv.asString();
    return true;
}

bool RedisService::setPost(const Post& p) const
{
    auto redis = pool.getConnection();

    bool res = redis->hmset(RedisKey::post(p.post_id),{
    {PostField::ID, std::to_string(p.post_id)},
    {PostField::USER_ID, std::to_string(p.user_id)},
    {PostField::AUTHOR, p.author},
    {PostField::TITLE, p.title},
    {PostField::CONTENT, p.content},
    {PostField::LIKE, std::to_string(p.like_count)},
    {PostField::COMMENT, std::to_string(p.comment_count)},
    {PostField::VIEW, std::to_string(p.view_count)},
    {PostField::TIME, p.create_time}
    });

    if(res) redis->expire(RedisKey::post(p.post_id), 1800);

    return res;
}

void RedisService::delPostPage() const
{
    auto redis = pool.getConnection();

    redis->delByPattern(RedisKey::postsPage());
}

bool RedisService::getPostPage(int page, int size, std::vector<int>& posts_id) const
{
    auto redis = pool.getConnection();
    std::vector<std::string> ids;
    if(redis->zrange(RedisKey::postIndex(), ids, (page-1)*size, page*size-1))
    {
        for(auto &i:ids)
        {
            posts_id.emplace_back(std::stoi(i));
        }
        return true;
    }
    return false;
}

bool RedisService::setPosts(const std::vector<Post>& posts) const
{
    auto redis = pool.getConnection();

    std::vector<std::pair<double, std::string>> members;
    for(const auto &i:posts)
    {
        members.emplace_back(static_cast<double>(StringToDatetime(i.create_time)), std::to_string(i.post_id));
        setPost(i);
    }

    redis->zadd(RedisKey::postIndex(), members);
    redis->expire(RedisKey::postIndex(), 1800);

    return true;
}

bool RedisService::getPost(int post_id, Post& p) const
{
    auto redis = pool.getConnection();

    std::unordered_map<std::string,std::string> fields;

    if(!redis->hgetAll(RedisKey::post(post_id), fields))
        return false;

    p.post_id       = std::stoi(fields[PostField::ID]);
    p.user_id       = std::stoi(fields[PostField::USER_ID]);
    p.author        = fields[PostField::AUTHOR];
    p.title         = fields[PostField::TITLE];
    p.content       = fields[PostField::CONTENT];
    p.like_count    = std::stoi(fields[PostField::LIKE]);
    p.comment_count = std::stoi(fields[PostField::COMMENT]);
    p.view_count    = std::stoi(fields[PostField::VIEW]);
    p.create_time   = fields[PostField::TIME];

    return true;
}

bool RedisService::getPosts(
    int page, int size, 
    std::vector<Post>& posts) const
{
    auto redis = pool.getConnection();

    std::vector<std::string> posts_id; 
    if(!redis->lrange(RedisKey::postsPage(page, size), posts_id))
        return false;

    for(auto &i:posts_id)
    {
        Post p;
        getPost(std::stoi(i), p);
        posts.emplace_back(std::move(p));
    }

    return true;
}

bool RedisService::getPostCount(int user_id, int& count) const
{
    auto redis = pool.getConnection();

    RedisValue rv = redis->hget(RedisKey::userStat(user_id),
        UserField::POST_COUNT
    );

    if(!rv.isInteger()) return false;

    count = rv.asInt();
    return true;
}

bool RedisService::setPostCount(int user_id, const int count) const
{
    auto redis = pool.getConnection();

    bool res = redis->hmset(RedisKey::userStat(user_id),{
    {UserField::POST_COUNT, std::to_string(count)}
    });
    if(res) redis->expire(RedisKey::userStat(user_id), 1800);
    return res;
}

bool RedisService::getCommentCount(int user_id, int& count) const
{
    auto redis = pool.getConnection();

    RedisValue rv = redis->hget(RedisKey::userStat(user_id),
        UserField::COMMENT_COUNT
    );

    if(!rv.isInteger()) return false;

    count = rv.asInt();
    return true;
}

bool RedisService::setCommentCount(int user_id, const int count) const
{
    auto redis = pool.getConnection();

    bool res = redis->hmset(RedisKey::userStat(user_id),{
    {UserField::COMMENT_COUNT, std::to_string(count)}
    });
    if(res) redis->expire(RedisKey::userStat(user_id), 1800);
    return res;
}

bool RedisService::getLikeCount(int user_id, int& count) const
{
    auto redis = pool.getConnection();

    RedisValue rv = redis->hget(RedisKey::userStat(user_id),
        UserField::LIKE_COUNT
    );

    if(!rv.isInteger()) return false;

    count = rv.asInt();
    return true;
}

bool RedisService::setLikeCount(int user_id, const int count) const
{
    auto redis = pool.getConnection();

    bool res = redis->hmset(RedisKey::userStat(user_id),{
    {UserField::LIKE_COUNT, std::to_string(count)}
    });
    if(res) redis->expire(RedisKey::userStat(user_id), 1800);
    return res;
}

bool RedisService::setComment(int comment_id, const Comment& c) const
{
    auto redis = pool.getConnection();

    bool res = redis->hmset(RedisKey::comment(comment_id),{
    {CommentField::ID, std::to_string(c.comment_id)},
    {CommentField::POST_ID, std::to_string(c.post_id)},
    {CommentField::USER_ID, std::to_string(c.user_id)},
    {CommentField::PARENT_ID, std::to_string(c.parent_id)},
    {CommentField::REPLY_USER_ID, std::to_string(c.reply_user_id)},

    {CommentField::AUTHOR, c.author},
    {CommentField::REPLY_AUTHOR, c.reply_author},

    {CommentField::CONTENT, c.content},
    {CommentField::CREATE_TIME, c.create_time}
    });
    if(res) redis->expire(RedisKey::comment(comment_id), 1800);
    return res;
}

bool RedisService::setComments(
    int post_id, int page, int size,
    const std::vector<Comment>& comments) const
{
    auto redis = pool.getConnection();

    std::vector<std::string> comments_id;
    comments_id.reserve(comments.size());

    for(const auto & c : comments)
    {
        setComment(c.comment_id, c);
        comments_id.emplace_back(std::to_string(c.comment_id));
    }

    return redis->rpush(
        RedisKey::commentsPage(post_id),
        comments_id);
}

bool RedisService::setComments(
    int post_id,
    const std::vector<Comment>& comments) const
{
    auto redis = pool.getConnection();

    std::vector<std::string> comments_id;
    comments_id.reserve(comments.size());

    for(const auto & c : comments)
    {
        setComment(c.comment_id, c);
        comments_id.emplace_back(std::to_string(c.comment_id));
    }

    return redis->rpush(
        RedisKey::postComments(post_id),
        comments_id);
}

bool RedisService::getComment(int comment_id, Comment& c) const
{
    auto redis = pool.getConnection();

    std::unordered_map<std::string,std::string> fields;

    if(!redis->hgetAll(RedisKey::comment(comment_id), fields))
        return false;

    c.comment_id      = std::stoi(fields[CommentField::ID]);
    c.post_id         = std::stoi(fields[CommentField::POST_ID]);          //帖子的id
    c.user_id         = std::stoi(fields[CommentField::USER_ID]);          //评论用户id

    c.parent_id       = std::stoi(fields[CommentField::PARENT_ID]);       //父评论id
    c.reply_user_id   = std::stoi(fields[CommentField::REPLY_USER_ID]);       //父评论用户id

    c.author          = fields[CommentField::AUTHOR];          //当前评论作者
    c.reply_author    = fields[CommentField::REPLY_AUTHOR];          //父评论作者
    c.content         = fields[CommentField::CONTENT];          //文本内容
    c.create_time     = fields[CommentField::CREATE_TIME];

    return true;
}

bool RedisService::getComments(
    int post_id, int page, int size, 
    std::vector<Comment>& comments) const
{
    auto redis = pool.getConnection();

    std::vector<std::string> comments_id; 
    int offset=(page-1)*size;

    if(!redis->lrange(
            RedisKey::commentsPage(post_id), 
            comments_id, 
            offset, 
            offset+(size-1)))
        return false;

    for(auto &i:comments_id)
    {
        Comment c;
        getComment(std::stoi(i), c);
        comments.emplace_back(std::move(c));
    }

    return true;
}

bool RedisService::getComments(
    int post_id,
    std::vector<Comment>& comments) const
{
    auto redis = pool.getConnection();

    std::vector<std::string> comments_id; 

    if(!redis->lrange(
            RedisKey::postComments(post_id), 
            comments_id
            ))
        return false;

    for(auto &i:comments_id)
    {
        Comment c;
        getComment(std::stoi(i), c);
        comments.emplace_back(std::move(c));
    }

    return true;
}

bool RedisService::updatePost(
    int post_id, 
    const std::string& title,
    const std::string& content) const
{
    auto redis = pool.getConnection();

    return redis->hmset(RedisKey::post(post_id),{
    {PostField::TITLE, title},
    {PostField::CONTENT, content}
    });
}

bool RedisService::checkPost(
    int post_id, 
    int user_id) const
{
    auto redis = pool.getConnection();

    RedisValue rv = redis->hget(RedisKey::post(post_id),
        PostField::USER_ID
    );

    if(!rv.isString()) return false;

    return rv.asString() == std::to_string(user_id);
}

bool RedisService::delPost(int post_id) const
{
    auto redis = pool.getConnection();

    return redis->del(RedisKey::post(post_id));
}

bool RedisService::incrLike(int post_id) const
{
    auto redis = pool.getConnection();

    return redis->hincr(RedisKey::post(post_id), PostField::LIKE, INCR);
}

bool RedisService::decrLike(int post_id) const
{
    auto redis = pool.getConnection();

    return redis->hincr(RedisKey::post(post_id), PostField::LIKE, DECR);
}

bool RedisService::getLikes(int post_id, int& like_count) const
{
    auto redis = pool.getConnection();

    RedisValue rv = redis->hget(RedisKey::post(post_id), PostField::LIKE);

    if(!rv.isInteger()) return false;
    
    like_count = rv.asInt();
    return true;
}

bool RedisService::incrComment(int post_id) const
{
    auto redis = pool.getConnection();

    return redis->hincr(RedisKey::post(post_id), PostField::COMMENT, INCR);
}

bool RedisService::decrComment(int post_id) const
{
    auto redis = pool.getConnection();

    return redis->hincr(RedisKey::post(post_id), PostField::COMMENT, DECR);
}

bool RedisService::incrView(int post_id) const
{
    auto redis = pool.getConnection();
    
    return redis->hincr(RedisKey::post(post_id), PostField::VIEW, INCR);
}


bool RedisService::addLikeUser(int post_id, int user_id) const
{
    auto redis = pool.getConnection();
    return redis->sadd(RedisKey::postLikes(post_id), RedisKey::user(user_id));
}

bool RedisService::removeLikeUser(int post_id, int user_id) const
{
    auto redis = pool.getConnection();
    return redis->srem(RedisKey::postLikes(post_id), RedisKey::user(user_id));
}

bool RedisService::hasLiked(int post_id, int user_id) const
{
    auto redis = pool.getConnection();

    return redis->sismember(RedisKey::postLikes(post_id), RedisKey::user(user_id));
}

bool RedisService::addDirty(int post_id) const
{
    auto redis = pool.getConnection();

    return redis->sadd(RedisKey::dirtyPost(), std::to_string(post_id));
}

bool RedisService::getDirty(std::string& post_id) const
{
    auto redis = pool.getConnection();

    return redis->spop(RedisKey::dirtyPost(), post_id);
}

// bool RedisService::getDirty(std::vector<std::string>& dirtyPosts) const
// {
//     auto redis = pool.getConnection();

//     bool res = redis->smembers(RedisKey::dirtyPost(), dirtyPosts);

//     redis->del(RedisKey::dirtyPost());

//     return res;
// }

bool RedisService::getViewLikeComment(int post_id, std::unordered_map<std::string, std::string>& fields) const
{
    auto redis = pool.getConnection();

    const std::vector<std::string> FLUSH = {PostField::VIEW, PostField::LIKE, PostField::COMMENT};
    for(auto &i:FLUSH)
    {
        fields[i] = "-1";
    }

    return redis->hmget(RedisKey::post(post_id), fields);
}

bool RedisService::setPostView(int post_id, int user_id)
{
    auto redis = pool.getConnection();

    bool res = redis->set(RedisKey::postView(post_id, user_id), "1");
    if(res) redis->expire(RedisKey::postView(post_id, user_id), 3600);

    return res;
}

bool RedisService::existPostView(int post_id, int user_id)
{
    auto redis = pool.getConnection();

    return redis->exists(RedisKey::postView(post_id, user_id));
}