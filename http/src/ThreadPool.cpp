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
        ThreadPool(int _size = 3)
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

        ~ThreadPool()
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

        template<class F,class... Args>
        void enqueue(F &&f,Args&&... args)
        {
            function<void()> task = bind(forward<F>(f), forward<Args>(args)...);
            {
                unique_lock<mutex> ul(mtx);
                tasks.emplace(move(task));
            }
            cv.notify_one();
        }
};