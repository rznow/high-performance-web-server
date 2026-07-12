#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <unordered_map>
#include <mutex>
#include "Post.h"

struct ListNode
{
    Post p;
    ListNode *pre = nullptr,*next = nullptr;

    ListNode():p(),pre(nullptr),next(nullptr){}
    ListNode(Post _p):p(_p),pre(nullptr),next(nullptr){}
};

class PostCache
{
    private:
        std::unordered_map<int, ListNode*> cache;
        ListNode *dummyHead, *dummyEnd;
        int capcity;
        int count;
        std::mutex mtx;
        PostCache();
        PostCache(int _capcity);
        ~PostCache();
        void printPosts();
    public:
        PostCache(const PostCache&) = delete;
        PostCache& operator=(const PostCache&) = delete;


        static PostCache& getInstance();
        bool get(int post_id, Post& post);
        void put(const Post& p);
        ListNode* removeNode(int post_id);
        void addToHead(ListNode* node);
        void removeEnd();
        void erase(size_t post_id);
        
};

#endif