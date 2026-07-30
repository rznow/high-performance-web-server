#include <unistd.h>
#include <atomic>
#include <vector>
#include <iostream>
#include <memory>
#include <thread>

#include "network/Connection.h"
#include "network/Reactor.h"
#include "network/Acceptor.h"
#include "thread/ThreadPool.hpp"
using namespace std;

constexpr int PORT = 8080;
constexpr int MAXEVENTS = 1000;
constexpr int MAIN_REACTOR_NUM = 1;
constexpr int SUB_REACTOR_NUM  = 4;
constexpr int THREAD_POOL_NUM  = 8;

int main()
{
    Acceptor acceptor(PORT);

    Reactor master;
    master.pushListen(acceptor.fd());

    //创建子Reactor序列
    vector<unique_ptr<Reactor>> subReactors;
    subReactors.reserve(SUB_REACTOR_NUM);
    vector<thread> threads;

    //创建线程池
    ThreadPool threadpool(THREAD_POOL_NUM);

    for(int i=0;i < SUB_REACTOR_NUM; i++)
    {
        subReactors.emplace_back(make_unique<Reactor>(MAXEVENTS, i, &threadpool));
        threads.emplace_back([&subReactors,i]{subReactors[i]->workloop();});
    }

    atomic<int> next{0};
    
    while(1)
    {
        //主reactor中的epoll轮询
        int nready = master.wait();
        for(int i = 0; i < nready; ++i)
        {
            if(master[i].data.fd != acceptor.fd())
                continue;
            //处理新连接
            while(true)         //一次就绪遍历所有就绪的连接事件
            {
                int new_fd = acceptor.acceptConnection();

                if(new_fd < 0)
                {
                    if(errno == EAGAIN ||
                    errno == EWOULDBLOCK)
                        break;

                    perror("accept");
                    break;
                }else if(new_fd > 0)
                {
                    //分给子reactor进行处理
                    // const char message[] = "connect to server!\n";
                    // write(new_fd, message, sizeof(message));

                    //通过轮询来为子Reactor添加socket端口
                    int idx = next.fetch_add(1) % SUB_REACTOR_NUM;

                    subReactors[idx]->push(new_fd);

                    // string msg = "assigned to No."+to_string(idx)+" Reactor!\n";
                    // write(new_fd, msg.c_str(), msg.length());
                }
            }
        }
    }
}
