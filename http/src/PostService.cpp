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

std::vector<Post> PostService::getPosts(size_t page, size_t size)
{
    auto mysql = pool.getConnection();

    std::vector<Post> posts;
    mysql->getPosts(posts, size, (page-1)*size);

    return posts;
}