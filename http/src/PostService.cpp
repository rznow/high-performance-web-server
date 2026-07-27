#include "service/PostService.h"
#include "service/RedisService.h"
#include "mysql/MySQL.h"
#include "common/Post.h"
#include "common/Comment.h"
#include "common/PostCache.h"
#include <iostream>

PostService::PostService():
    cache(PostCache::getInstance()),
    pool(MySQLPool::getInstance()){}

PostService::~PostService(){}

PostService& PostService::getInstance()
{
    static PostService ps;
    return ps;
}

//MySQL → Redis → PostCache
void PostService::put(Post p)
{
    auto mysql = pool.getConnection();

    mysql->savePost(p);
    
    RedisService::getInstance().setPost(p.post_id, p);

    PostCache::getInstance().put(p);
}

//MySQL → Redis → PostCache
void PostService::put(Comment& c)
{
    auto mysql = pool.getConnection();

    mysql->saveComment(c);

    RedisService::getInstance().setComment(c.comment_id, c);

    PostCache::getInstance().update(c);
}

//PostCache → Redis → MySQL
bool PostService::get(int post_id, Post& p)
{
    if(PostCache::getInstance().get(post_id, p))
    {
        std::cout<<"Post from postCache!"<<std::endl;
        return true;
    }

    if(RedisService::getInstance().getPost(post_id, p))
    {
        std::cout<<"Post from redis!"<<std::endl;
        PostCache::getInstance().put(p);
        return true;
    }

    auto mysql = pool.getConnection();

    if(mysql->getPost(post_id, p))
    {
        std::cout<<"Post from mysql!"<<std::endl;
        PostCache::getInstance().put(p);
        RedisService::getInstance().setPost(p.post_id, p);
        return true;
    }
    
    return false;
}

//MySQL → Redis → PostCache
int PostService::like(int post_id, int user_id, bool& liked)    //点赞和取消点赞
{
    auto mysql = pool.getConnection();

    int res = mysql->like(post_id, user_id, liked);

    if(liked) 
    {
        RedisService::getInstance().addLikeUser(post_id, user_id);
        RedisService::getInstance().incrLike(post_id);
    }
    else 
    {
        RedisService::getInstance().removeLikeUser(post_id, user_id);
        RedisService::getInstance().decrLike(post_id);
    }

    PostCache::getInstance().update(post_id, liked);

    return res;
}

//Redis -> MySQL
bool PostService::liked(int post_id, int user_id)               //是否点赞
{
    auto mysql = pool.getConnection();
    
    if(RedisService::getInstance().hasLiked(post_id, user_id))
    {
        return true;
    }

    if(mysql->liked(post_id, user_id))
    {
        RedisService::getInstance().addLikeUser(post_id, user_id);
        return true;
    }

    return false;
}

//Redis -> Mysql
std::vector<Post> PostService::getPosts(size_t page, size_t size)
{
    std::vector<Post> posts;

    if(RedisService::getInstance().getPosts(page, size, posts))
        return posts;

    auto mysql = pool.getConnection();

    mysql->getPosts(posts, size, (page - 1) * size);

    RedisService::getInstance().setPosts(page, size, posts);

    return posts;
}

//Redis -> Mysql
std::vector<Comment> PostService::getRootComments(size_t post_id, size_t page, size_t size)
{
    auto& redis = RedisService::getInstance();

    std::vector<Comment> comments;

    if(redis.getComments(post_id, page, size, comments))
        return comments;

    auto mysql = pool.getConnection();

    mysql->getRootComments(
        comments,
        post_id,
        size,
        (page-1)*size);

    redis.setComments(post_id, page, size, comments);

    return comments;
}

//Redis -> Mysql
std::vector<Comment> PostService::getComments(size_t post_id)
{
    auto& redis = RedisService::getInstance();

    std::vector<Comment> comments;

    if(redis.getComments(post_id, comments))
        return comments;

    auto mysql = pool.getConnection();

    mysql->getComments(comments, post_id);

    redis.setComments(post_id, comments);

    return comments;
}

//MySQL → Redis → PostCache
bool PostService::delPost(size_t post_id)
{
    auto mysql = pool.getConnection();

    bool res = mysql->delPost(post_id);

    RedisService::getInstance().delPost(post_id);

    PostCache::getInstance().erase(post_id);

    return res;
}

//MySQL → Redis → PostCache
void PostService::modifyView(size_t post_id)
{
    auto mysql = pool.getConnection();

    mysql->view(post_id);

    RedisService::getInstance().incrView(post_id);

    PostCache::getInstance().update(post_id);
}

//MySQL → Redis → PostCache
int PostService::modPost(size_t post_id, size_t user_id, std::string& title, std::string& content)
{
    auto mysql = pool.getConnection();

    if(checkPost(post_id, user_id))
    {
        if(mysql->modPost(post_id, title, content))
        {
            RedisService::getInstance().updatePost(post_id, title, content);
            PostCache::getInstance().update(post_id, title, content);
            return 0;
        }
        return 2; //修改失败
    }
    return 1;  //帖子并非当前用户所有
}

//PostCache → Redis → MySQL
bool PostService::checkPost(size_t post_id, size_t user_id)         //判断帖子是否为本人的
{
    /*
    
    PostCache
    
    */
    if(RedisService::getInstance().checkPost(post_id, user_id))
    {
        return true;
    }

    auto mysql = pool.getConnection();
    return mysql->checkPost(post_id, user_id);
}

