#include "service/PostService.h"
#include "mysql/MySQL.h"
#include <iostream>

// 
PostService::PostService():cache(PostCache::getInstance()),pool(MySQLPool::getInstance())
{

}
PostService::~PostService(){}

PostService& PostService::getInstance()
{
    static PostService ps;
    return ps;
}

void PostService::put(Post p)
{
    auto mysql = pool.getConnection();

    mysql->savePost(p);
    
    PostCache::getInstance().put(p);

}

bool PostService::get(int post_id, Post& p)
{
    auto mysql = pool.getConnection();

    bool res = mysql->getPost(post_id, p);
    
    PostCache::getInstance().put(p);

    return res;
}

int PostService::like(int post_id, int user_id, bool& liked)
{
    auto mysql = pool.getConnection();

    int res = mysql->like(post_id, user_id, liked);

    PostCache::getInstance().update(post_id, liked);

    return res;
}

bool PostService::liked(int post_id, int user_id)
{
    auto mysql = pool.getConnection();
    
    return mysql->liked(post_id, user_id);
}

std::vector<Post> PostService::getPosts(size_t page, size_t size)
{
    auto mysql = pool.getConnection();

    std::vector<Post> posts;
    mysql->getPosts(posts, size, (page-1)*size);
    for(auto &p: posts) PostCache::getInstance().put(p);
    return posts;
}

bool PostService::delPost(size_t post_id)
{
    auto mysql = pool.getConnection();

    bool res = mysql->delPost(post_id);
    
    PostCache::getInstance().erase(post_id);

    return res;
}

void PostService::modViewCount(size_t post_id)
{
    auto mysql = pool.getConnection();

    mysql->view(post_id);
}

int PostService::modPost(size_t post_id, size_t user_id, std::string& title, std::string& content)
{
    auto mysql = pool.getConnection();

    if(mysql->checkPost(post_id, user_id))
    {
        if(mysql->modPost(post_id, title, content))
        {
            PostCache::getInstance().update(post_id, content);
            return 0;
        }
        return 2; //修改失败
    }
    return 1;  //帖子并非当前用户所有
}