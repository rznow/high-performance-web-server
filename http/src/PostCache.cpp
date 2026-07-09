#include "common/PostCache.h"

PostCache::~PostCache(){}

PostCache& PostCache::getInstance()
{
    static PostCache cache;
    return cache;
}

PostCache::PostCache()
{
    dummyHead = new ListNode();
    dummyEnd = new ListNode();
    dummyHead->next = dummyEnd;
    dummyEnd->pre = dummyHead;
    capcity = 10;
    count = 0;
}
PostCache::PostCache(int _capcity)
{
    dummyHead = new ListNode();
    dummyEnd = new ListNode();  
    dummyHead->next = dummyEnd;
    dummyEnd->pre = dummyHead;
    capcity = _capcity;
    count = 0;
}

Post PostCache::get(int post_id)
{
    std::unique_lock<std::mutex> ul(mtx);
    if(cache.find(post_id)!=cache.end())
    {
        std::unique_lock<std::mutex> ul(mtx);
        ListNode *node = removeNode(post_id);
        addToHead(node);
        return node->p;
    }else
    {
        Post p{-1};
        return p;
    }
}

void PostCache::put(const Post& p)
{
    std::unique_lock<std::mutex> ul(mtx);
    if(cache.find(p.post_id)!=cache.end())
    {
        cache[p.post_id]->p = p;
        ListNode* node = removeNode(p.post_id);
        addToHead(node);
    }else
    {
        ListNode* newNode = new ListNode(p);
        addToHead(newNode);
        cache[p.post_id] = newNode;
        count++;

        if(count > capcity)
        {
            count--;
        }
    }
    ul.unlock();
    std::cout<<"count:\t"<<count<<std::endl;
    
    printPosts();
    
}

ListNode* PostCache::removeNode(int key)
{
    ListNode *node = cache[key];
    node->pre->next = node->next;
    node->next->pre = node->pre;

    return node;
}

void PostCache::addToHead(ListNode* node)
{
    dummyHead->next->pre = node;
    node->next = dummyHead->next;
    dummyHead->next = node;
    node->pre = dummyHead;
}

void PostCache::removeEnd()
{
    ListNode* del = removeNode(dummyEnd->pre->p.post_id);
    cache.erase(del->p.post_id);
    delete del;
}

void PostCache::printPosts()
{
    ListNode *cur = dummyHead->next;
    while(cur != dummyEnd)
    {
        cur->p.print();
        cur = cur->next;
    }   
}