#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>

#include "MPMC.hpp"

class MPMCThreadPool
{
    private:
        MPMCQueue<std::function<void()>> tasks;
        std::atomic<size_t> pending{0};
        std::vector<std::thread> threads;
        std::atomic<bool> stop_;
    public:
        MPMCThreadPool(int _size = 3);

        ~MPMCThreadPool();

        template<class F,class... Args>
        void enqueue(F &&f,Args&&... args)
        {
            std::function<void()> task = bind(std::forward<F>(f), std::forward<Args> (args)...);

            if(stop_.load(std::memory_order_acquire)) return;
            tasks.Enqueue(std::move(task));
            pending.fetch_add(1, std::memory_order_release);
            pending.notify_one();
        }
};
