#pragma once 

#include "redis/RedisPool.h"
#include "common/Post.h"


class RedisService
{
    private:
        RedisPool& pool;

        RedisService();
        ~RedisService();
        
    public:
        RedisService(const RedisService&) = delete;
        RedisService& operator=(const RedisService&) = delete;
        static RedisService& getInstance();

        bool setPost(int post_id, Post& p) const;
        bool getPost(int post_id, Post& p) const;
        bool updatePost(
            int post_id, 
            const std::string& title,
            const std::string& content) const;

        bool incrLike(int post_id) const;

        bool decrLike(int post_id) const;

        bool incrComment(int post_id) const;

        bool decrComment(int post_id) const;

        bool incrView(int post_id) const;

        bool addLikeUser(int post_id, int user_id) const;

        bool removeLikeUser(int post_id, int user_id) const;

        bool hasLiked(int post_id, int user_id) const;
};