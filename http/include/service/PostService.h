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
        std::vector<Post> getPosts();
};




#endif