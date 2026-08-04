#include <unistd.h>
#include <atomic>
#include <vector>
// #include <iostream>
#include <memory>
#include <thread>

#include "network/Connection.h"
#include "network/Reactor.h"
#include "network/Acceptor.h"
#include "thread/ThreadPool.hpp"
#include "redis/RedisPool.h"
#include "mysql/MySQLPool.h"
#include "util/Config.h"

int main()
{
    //读取配置文件
    Config cfg;
    cfg.load("./server.conf");

    MySQLPool::getInstance().init(cfg);
    RedisPool::getInstance().init(cfg);

    Acceptor acceptor(cfg.getInt("port", 8080));

    Reactor master;
    master.pushListen(acceptor.fd());

    //创建子Reactor序列
    vector<unique_ptr<Reactor>> subReactors;
    subReactors.reserve(cfg.getInt("reactor_num", 4));
    vector<thread> threads;

    //创建线程池
    ThreadPool threadpool(cfg.getInt("thread_num", 8));
    
    for(int i=0;i < cfg.getInt("reactor_num", 4); i++)
    {
        subReactors.emplace_back(make_unique<Reactor>(cfg.getInt("max_events"), i, &threadpool));
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
                    int idx = next.fetch_add(1) % cfg.getInt("reactor_num", 4);

                    subReactors[idx]->push(new_fd);

                    // string msg = "assigned to No."+to_string(idx)+" Reactor!\n";
                    // write(new_fd, msg.c_str(), msg.length());
                }
            }
        }
    }
}
