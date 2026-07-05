#include "thread/ThreadPool.hpp"

ThreadPool::ThreadPool(int _size)
{
    stop = false;

    for(int i=0;i < _size;i++)
    {
        threads.emplace_back([this]{
                while(1)
                {
                    function<void()> task;
                    {
                        unique_lock<mutex> ul(mtx);
                        cv.wait(ul,[this]{return !tasks.empty()||stop;});
                        if(tasks.empty()&&stop)    break;
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            }
        );
    }
}

ThreadPool::~ThreadPool()
{
    {   
        unique_lock<mutex> ul(mtx);
        stop = true;
    }

    cv.notify_all();
    for(auto &t:threads)
    {
        t.join();
    }
}