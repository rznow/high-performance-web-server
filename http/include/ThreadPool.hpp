#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <queue>
#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>
using namespace std;

class ThreadPool
{
    private:
        mutex mtx;
        queue<function<void()>> tasks;
        condition_variable cv;
        vector<thread> threads;
        bool stop;
    public:
        ThreadPool(int _size = 3);

        ~ThreadPool();

        template<class F,class... Args>
        void enqueue(F &&f,Args&&... args)
        {
            function<void()> task = bind(forward<F>(f), forward<Args> (args)...);
            {
                unique_lock<mutex> ul(mtx);

                if(stop) return;

                tasks.emplace(move(task));
            }

            cv.notify_one();
        }
};


#endif