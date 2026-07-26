#include "service/RedisService.h"
#include "common/RedisConst.h"
#include "redis/Redis.h"
#include <unordered_map>

using namespace PostField;
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



bool RedisService::setPost(int post_id, Post& p) const
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