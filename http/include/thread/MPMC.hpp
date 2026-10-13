#pragma once

#include <atomic>
#include <cstddef>
#include <utility>
#include <array>
#include <unordered_set>

const int MAX_THREADS = 8;
const int MAX_HP_PER_THREAD = 2;

template <class T>
class MPMCQueue
{   
    struct Element;
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

        bool Dequeue(T& value, int thread_id) {  
            auto hps = hazard_pointers[thread_id];
            while(true)
            {
                // 1.读取栈顶节点
                Element* oldHead = head_.load(std::memory_order_acquire);
                // 存入HP槽位，声明当前节点在用
                hps[0].store(oldHead, std::memory_order_release); 
                // 2. 判断栈顶是否变化
                if(head_.load(std::memory_order_acquire) != oldHead)
                {
                    continue;
                } 
                // 3. CAS 更新 栈顶结点改为next
                Element* next = oldHead->next.load(std::memory_order_acquire);
                if(next == nullptr) return false;

                if(head_.compare_exchange_strong(
                    oldHead, next,
                    std::memory_order_release,
                    std::memory_order_relaxed))
                {
                    // 4. 释放HP槽位
                    hps[0].store(nullptr, std::memory_order_release);
                    // 5. 将旧节点放入退役列表
                    retire_node(oldHead);
                    value = std::move(next->data);
                    // 6.  当退役列表长度达标时触发回收
                    if (retired_count > MAX_THREADS * MAX_HP_PER_THREAD) {
                        reclaim_nodes();
                    }
                    return true;
                }
            }
        }  

        bool empty()
        {
            Element* oldHead = head_.load(std::memory_order_relaxed);  
            return oldHead->next.load(std::memory_order_acquire) != nullptr;
        }

        void retire_node(Element* e) {
            Element* oldHead = retired_list.load(std::memory_order_acquire);
            do {
                e->next.store(oldHead, std::memory_order_release);
            } while (!retired_list.compare_exchange_weak(oldHead, e, std::memory_order_release));

            retired_count.fetch_add(1, std::memory_order_relaxed);
        }

        size_t get_retired_list_size() {
            size_t size = 0;
            Element* current = retired_list.load(std::memory_order_acquire);
            while (current != nullptr) {
                size++;
                current = current->next.load(std::memory_order_acquire);
            }
            return size;
        }

        void reclaim_nodes() {
            // 步骤 1：收集所有活跃的 Hazard Pointer
            std::unordered_set<Element*> active_hps;
            for (int i = 0; i < MAX_THREADS; ++i) {
                for (int j = 0; j < MAX_HP_PER_THREAD; ++j) {
                    Element* hp = hazard_pointers[i][j].load(std::memory_order_acquire);
                    if (hp != nullptr) {
                        active_hps.insert(hp);
                    }
                }
            }
        
            // 步骤 2：遍历退役列表，释放不在活跃 HP 中的节点
            Element* current = retired_list.exchange(nullptr, std::memory_order_acquire);
            while (current != nullptr) {
                Element* next = current->next.load(std::memory_order_acquire);
                if (active_hps.find(current) == active_hps.end()) {
                    delete current; // 安全释放
                    retired_count.fetch_sub(1, std::memory_order_release);
                } else {
                    // 仍被访问，重新加入退役列表
                    retire_node(current);
                }
                current = next;
            }
        }

        std::atomic<Element*>** my_hps()
        {
            return hazard_pointers;
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

        std::atomic<Element*> hazard_pointers[MAX_THREADS][MAX_HP_PER_THREAD];
        // 退役列表
        std::atomic<Element*> retired_list = nullptr;

        std::atomic<size_t> retired_count{0};
};