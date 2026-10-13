#include <iostream>
#include <thread>
#include <string>
#include <mutex>

#include "SPSC.h"

int main()
{
    SPSCQueue<int, 10> q;
    std::atomic<bool> done{false};
    std::string out;
    std::mutex mtx;

    std::thread c([&]{
        int t;
        while(1)
        {
            if(q.Dequeue(t))
            {   
                mtx.lock();
                out += "consume:\t" + std::to_string(t) + '\n';
                mtx.unlock();
            }
                // std::cout<<"consume:\t"<<t<<std::endl;
            else if(done.load(std::memory_order_acquire))
                break;
        }
    });

    std::thread p([&]{
        for(int i=1;i <= 9;i++)
        {
            if(q.Enqueue(i)==true) 
            {
                mtx.lock();
                out += "produce:\t" + std::to_string(i) + '\n';
                mtx.unlock();
                // std::cout<<"produce:\t"<<i<<std::endl;
            }
        }
        done.store(true, std::memory_order_release);
    });

    c.join();
    p.join();

    std::cout<<out<<std::endl;

    return 0;
}
