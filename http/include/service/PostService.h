#ifndef POSTSERVICE_H
#define POSTSERVICE_H

#include "common/PostCache.h"
#include "mysql/MySQLPool.h"
class Post;

class PostService
{
    private:
        PostCache& cache;
        MySQLPool& pool;

        PostService();
        ~PostService();
        
    public:
        PostService(const PostService&) = delete;
        PostService& operator=(const PostService&) = delete;
        static PostService& getInstance();
        void put(Post p);
        bool get(int post_id, Post& p);
        int like(int post_id, int user_id);
        bool liked(int post_id, int user_id);
        std::vector<Post> getPosts(size_t page, size_t size);  
        bool delPost(size_t post_id);
};




#endif