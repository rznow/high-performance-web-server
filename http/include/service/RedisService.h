#pragma once 

#include "redis/RedisPool.h"
#include "common/Post.h"
#include "common/Comment.h"
#include <unordered_map>

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

        bool setPost(int post_id, const Post& p) const;
        void delPostPage() const;
        bool setPosts(int page, int size, const std::vector<Post>& posts) const;
        bool getPost(int post_id, Post& p) const;
        bool getPosts(int page, int size, std::vector<Post>& posts) const;

        bool setComment(int comment_id, const Comment& c) const;
        bool setComments(int post_id, const std::vector<Comment>& comments) const;
        bool setComments(
            int post_id, int page, int size, 
            const std::vector<Comment>& comments) const;
        bool getComment(int comment_id, Comment& c) const;
        bool getComments(int post_id, std::vector<Comment>& comments) const;
        bool getComments(
            int post_id, int page, int size, 
            std::vector<Comment>& comments) const;
        

        bool updatePost(
            int post_id, 
            const std::string& title,
            const std::string& content) const;

        bool checkPost(
            int post_id, 
            int user_id) const;

        bool existPostLikes(int post_id) const;

        bool delPost(int post_id) const;

        bool incrLike(int post_id) const;

        bool decrLike(int post_id) const;

        bool getLikes(int post_id, int& like_count) const;

        bool incrComment(int post_id) const;

        bool decrComment(int post_id) const;

        bool incrView(int post_id) const;

        bool addLikeUser(int post_id, int user_id) const;

        bool removeLikeUser(int post_id, int user_id) const;

        bool hasLiked(int post_id, int user_id) const;

        bool addDirty(int post_id) const;

        bool getDirty(std::string& post_id) const;

        bool getViewLikeComment(int post_id, std::unordered_map<std::string, std::string>& fields) const;
};