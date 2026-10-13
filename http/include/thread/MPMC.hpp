#pragma once

#include <atomic>
#include <cstddef>
#include <utility>
#include <array>

template <class T>
class MPMCQueue
{
    public:
        MPMCQueue()
        {
            dummy = new Element();
            head_.store(dummy, std::memory_order_relaxed);
            tail_.store(dummy, std::memory_order_relaxed);
        };

        MPMCQueue(MPMCQueue&&) noexcept = default;
        MPMCQueue& operator=(MPMCQueue&&) noexcept = default;
        MPMCQueue(const MPMCQueue&)  = delete;
        MPMCQueue& operator=(const MPMCQueue&) = delete;

        bool Enqueue(T&& value) {  
            Element* e = new Element(std::forward<T>(value));
            while(true)
            {
                Element* oldTail = tail_.load(std::memory_order_relaxed);
                Element* next = oldTail->next.load(std::memory_order_acquire);

                if(next == nullptr)
                {
                    if(oldTail->next.compare_exchange_weak(
                        next, e,
                        std::memory_order_release,
                        std::memory_order_relaxed))
                    {
                        //尝试更新tail指向新节点 (允许失败，其他线程会帮忙)
                        tail_.compare_exchange_strong(  
                            oldTail, e,  
                            std::memory_order_release,  
                            std::memory_order_relaxed);  
                        return true; 
                    }
                }else
                {
                    //帮助其他线程交换tail
                    //next不为空，所以newNode即结点已经迁移，所以不需要约束操作前后顺序，只需要完成索引的更新
                    tail_.compare_exchange_weak(
                        oldTail, next,
                        std::memory_order_relaxed,
                        std::memory_order_relaxed
                    );
                }
            }
            return false;
        }  

        bool Dequeue(T& value) {  
            Element* oldHead = head_.load(std::memory_order_relaxed);  
            while(true)
            {
                Element* next = oldHead->next.load(std::memory_order_acquire);
                if(next == nullptr) return false;

                if(head_.compare_exchange_strong(
                    oldHead, next,
                    std::memory_order_release,
                    std::memory_order_relaxed))
                {
                    value = std::move(next->data);
                    delete oldHead;
                    return true;
                }
            }
        }  

        bool empty()
        {
            Element* oldHead = head_.load(std::memory_order_relaxed);  
            Element* next = oldHead->next.load(std::memory_order_acquire);
            if(next == nullptr) return false;
            return true;
        }

    private:
        size_t capacity_;

        struct alignas(64) Element{
            std::atomic<Element*> next;
            T data;

            Element() = default;
            Element(T&& value) : next(nullptr), data(value){}
        };

        Element* dummy;

        alignas(64) std::atomic<Element*> head_;
        alignas(64) std::atomic<Element*> tail_;
};