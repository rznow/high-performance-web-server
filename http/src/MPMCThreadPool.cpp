#include "thread/MPMCThreadPool.hpp"

MPMCThreadPool::MPMCThreadPool(int _size)
{   
    
    stop_ = false;

    for(int i=0;i < _size;i++)
    {
        threads.emplace_back([this]{
                std::function<void()> task;
                while(1)
                {
                    if(tasks.Dequeue(task))
                    {
                        try{
                            task();
                        }catch(...){}
                        pending.fetch_sub(1, std::memory_order_release);
                        continue;
                    }
                    
                    if(pending.load(std::memory_order_acquire) == 0&&
                        stop_.load(std::memory_order_acquire))    
                        break;
                    
                    size_t n = pending.load(std::memory_order_acquire);

                    if (n == 0 &&
                        !stop_.load(std::memory_order_acquire))
                    {
                        pending.wait(0, std::memory_order_acquire);
                    }
                    
                }
            }
        );
    }
}

MPMCThreadPool::~MPMCThreadPool()
{
    bool expected = false;
    if(!stop_.compare_exchange_strong(expected, true)) return;

    pending.notify_all();

    for(auto &t:threads)
    {
        if(t.joinable())
            t.join();
    }
}