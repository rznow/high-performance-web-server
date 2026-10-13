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
        std::mutex mtx;
        MPMCQueue<std::function<void()>> tasks;
        std::condition_variable cv;
        std::vector<std::thread> threads;
        bool stop;
    public:
        MPMCThreadPool(int _size = 3);

        ~MPMCThreadPool();

        template<class F,class... Args>
        void enqueue(F &&f,Args&&... args)
        {
            std::function<void()> task = bind(std::forward<F>(f), std::forward<Args> (args)...);

            if(stop) return;
            tasks.Enqueue(std::move(task));

            cv.notify_one();
        }
};
