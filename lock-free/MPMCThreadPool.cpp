#include "MPMCThreadPool.hpp"

MPMCThreadPool::MPMCThreadPool(int _size)
{
    stop = false;

    for(int i=0;i < _size;i++)
    {
        threads.emplace_back([this]{
                while(1)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> ul(mtx);

                        cv.wait(ul,[this]{return !tasks.empty()||stop;});
                        if(tasks.empty()&&stop)    break;
                    }
                    if(tasks.Dequeue(task))
                        task(); 
                }
            }
        );
    }
}

MPMCThreadPool::~MPMCThreadPool()
{
    {   
        std::unique_lock<std::mutex> ul(mtx);
        stop = true;
    }

    cv.notify_all();
    for(auto &t:threads)
    {
        t.join();
    }
}