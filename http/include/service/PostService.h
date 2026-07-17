#ifndef POSTSERVICE_H
#define POSTSERVICE_H

#include "common/PostCache.h"
#include "mysql/MySQLPool.h"
class Post;
class Comment;

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
        void put(Comment& c);
        bool get(int post_id, Post& p);
        int like(int post_id, int user_id, bool& liked);
        bool liked(int post_id, int user_id);
        std::vector<Post> getPosts(size_t page, size_t size); 
        std::vector<Comment> getRootComments(size_t post_id, size_t page, size_t size);
        std::vector<Comment> getComments(size_t post_id);  
        bool delPost(size_t post_id);
        void modViewCount(size_t post_id);
        int modPost(size_t post_id, size_t user_id, std::string& title, std::string& content);
};




#endif