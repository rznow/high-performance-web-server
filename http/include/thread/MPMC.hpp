#pragma once

#include <atomic>
#include <cstddef>
#include <utility>
#include <array>
#include <unordered_set>
#include <condition_variable>
#include <thread>
#include <iostream>

const int MAX_THREADS = 8;
const int MAX_REACTORS = 4;
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

            clean_up = std::thread([this]{
                while(1)
                {
                    {
                        std::unique_lock<std::mutex> ul(mtx);
                        cv.wait(ul, [this]{
                            return cleanup_requested_ || stop_;
                        });

                        if(stop_) return;

                        cleanup_requested_ = false;
                    }

                    reclaim_nodes();

                }
                
            });

        };

        ~MPMCQueue()
        {
            {
                std::lock_guard<std::mutex> lock(mtx);
                stop_ = true;
            }

                cv.notify_one();

                if (clean_up.joinable()) {
                    clean_up.join();
            }
        };

        MPMCQueue(MPMCQueue&&) noexcept = default;
        MPMCQueue& operator=(MPMCQueue&&) noexcept = default;
        MPMCQueue(const MPMCQueue&)  = delete;
        MPMCQueue& operator=(const MPMCQueue&) = delete;

        bool Enqueue(T&& value, int reactor_id) {  
            Element* e = new Element(std::forward<T>(value));
            Element* oldTail;
            Element* next;

            while(true)
            {
                do {
                    oldTail = tail_.load(std::memory_order_acquire);
                    hps_tail[reactor_id].store(oldTail, std::memory_order_release);
                } while (tail_.load(std::memory_order_acquire) != oldTail);

                next = oldTail->next.load(std::memory_order_acquire);
                // // 判断next节点是否被抢占
                // if(oldTail->next.load(std::memory_order_acquire) != next)
                // {
                //     continue;
                // } 

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
            if (retired_count > MAX_THREADS * MAX_HP_PER_THREAD) {
                cleanUp();
            }
            auto hps = hazard_pointers[thread_id];
            Element* oldHead;
            Element* next;
            while(true)
            {
                do{
                    // 1.读取栈顶节点
                    oldHead = head_.load(std::memory_order_acquire);
                    // 存入HP槽位，声明当前节点在用
                    hps[0].store(oldHead, std::memory_order_release); 
                }while(head_.load(std::memory_order_acquire) != oldHead);    // 2. 判断栈顶是否变化
                
                // 3. CAS 更新 栈顶结点改为next
                Element* tail = tail_.load(std::memory_order_acquire);
                next = oldHead->next.load(std::memory_order_acquire);
                // 存入HP槽位，声明当前节点在用
                hps[1].store(next, std::memory_order_release);

                // 判断next节点是否被抢占

                if(next == nullptr)
                {
                    if(oldHead == tail) return false;
                    else 
                    {
                        hps[0].store(nullptr);
                        hps[1].store(nullptr);
                        continue;
                    }
                }else
                {
                    if(oldHead == tail)
                    {
                        tail_.compare_exchange_weak(
                            tail, next,
                            std::memory_order_relaxed,
                            std::memory_order_relaxed);
                        
                        hps[0].store(nullptr);
                        hps[1].store(nullptr);
                        continue;
                    }

                    if(head_.compare_exchange_strong(
                    oldHead, next,
                    std::memory_order_release,
                    std::memory_order_relaxed))
                    {
                        value = std::move(next->data);
                        // 4. 释放HP槽位
                        hps[0].store(nullptr, std::memory_order_release);
                        hps[1].store(nullptr, std::memory_order_release);
                        // 5. 将旧节点放入退役列表
                        retire_node(oldHead);
                        // // 6.  当退役列表长度达标时触发回收
                        // if (retired_count > MAX_THREADS * MAX_HP_PER_THREAD) {
                        //     cleanUp();
                        // }
                        return true;
                    }
                }
                hps[0].store(nullptr);
                hps[1].store(nullptr);
            } 
        }  
        
        void cleanUp()
        {
            {
                std::lock_guard<std::mutex> lock(mtx);
                cleanup_requested_ = true;
            }

            cv.notify_one();
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
            for (int i = 0; i < MAX_REACTORS; ++i) {
                Element* hp = hps_tail[i].load(std::memory_order_acquire);
                if (hp != nullptr) {
                    active_hps.insert(hp);
                }
            }
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
                retired_count.fetch_sub(1, std::memory_order_release);
                if (active_hps.find(current) == active_hps.end()) {
                    delete current; // 安全释放
                    // std::cout<<"退休列表长度："<<retired_count<<std::endl;
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
            Element(T&& value) : next(nullptr), data(std::forward<T>(value)){}
            // ~Element(){ 
            //     static std::atomic<int> count;
            //     count.fetch_add(1);
            //     // std::cout<<count<<std::endl;
            // }
        };

        Element* dummy;

        alignas(64) std::atomic<Element*> head_;
        alignas(64) std::atomic<Element*> tail_;

        std::atomic<Element*> hazard_pointers[MAX_THREADS][MAX_HP_PER_THREAD]{};
        std::atomic<Element*> hps_tail[MAX_REACTORS];
        // 退役列表
        std::atomic<Element*> retired_list = nullptr;

        std::atomic<size_t> retired_count{0};

        std::mutex mtx;
        std::thread clean_up;
        std::condition_variable cv;
        bool cleanup_requested_ = false;
        bool stop_ = false;
};