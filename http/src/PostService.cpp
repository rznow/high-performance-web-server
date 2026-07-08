#include "service/PostService.h"
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
    PostCache::getInstance().put(p);
}