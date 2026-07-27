#include "service/RedisService.h"
#include "common/RedisConst.h"
#include "redis/Redis.h"
#include <unordered_map>

using namespace PostField;
using namespace CommentField;
using namespace RedisKey;

constexpr std::string INCR = "1";
constexpr std::string DECR = "-1";

RedisService::RedisService():pool(RedisPool::getInstance()){}

RedisService::~RedisService(){}

RedisService& RedisService::getInstance()
{
    static RedisService redisService;
    return redisService;
}

bool RedisService::setPost(int post_id, const Post& p) const
{
    auto redis = pool.getConnection();

    return redis->hmset(RedisKey::post(post_id),{
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
}

bool RedisService::setPosts(int page, int size, const std::vector<Post>& posts) const
{
    auto redis = pool.getConnection();

    std::vector<std::string> posts_id;
    for(const auto &i:posts)
    {
        posts_id.emplace_back(RedisKey::post(i.post_id));
        setPost(i.post_id, i);
    }

    redis->lpush(RedisKey::postsPage(page, size), posts_id);

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

bool RedisService::getPosts(int page, int size, std::vector<Post>& posts) const
{
    auto redis = pool.getConnection();

    std::vector<std::string> posts_id; 
    if(!redis->lrange(RedisKey::postsPage(page, size), posts_id))
        return false;

    for(auto &i:posts_id)
    {
        Post p;
        getPost(RedisKey::deuser(i), p);
        posts.emplace_back(std::move(p));
    }

    return true;
}

bool RedisService::setComment(int comment_id, const Comment& c) const
{
    auto redis = pool.getConnection();

    return redis->hmset(RedisKey::comment(comment_id),{
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