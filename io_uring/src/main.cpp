#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <cstring>

#include <liburing.h>

#include "ioUring.h"
#include "acceptor.h"
#include "ioUringReactor.h"

int main()
{
    Acceptor acceptor(8080);

    IoUring master;

    // 准备 accept SQE
    master.prepAccept(
        acceptor.fd(),
        // (sockaddr*)&client_addr,
        // &client_addr_len,
        reinterpret_cast<uint64_t>(&acceptor)
    );

    master.submit();

    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<IoUringReactor>> subReactors;

    for(int i=0;i < 1; i++)
    {
        subReactors.emplace_back(std::make_unique<IoUringReactor>(1024, i));
        threads.emplace_back([&subReactors,i]{subReactors[i]->workloop();});
    }

    int idx = 0;
    while(1)
    {
        io_uring_cqe* cqe;
        if(master.waitCqe(&cqe) < 0)
            continue;

        if(cqe->user_data == reinterpret_cast<uint64_t>(&acceptor))
        {
            int fd = cqe->res;  //获取新连接

            if (fd < 0)
            {
                std::cerr << "accept failed: "<< strerror(-fd)<< "\n";
            }
            else
            {
                // std::cout << "new client fd: "<< fd << "\n";
                idx = (idx + 1) % 1;
                subReactors[idx]->push(fd);
            }
            // master.send(
            //     fd,
            //     s.data(),
            //     s.length(),
            //     0,
            //     reinterpret_cast<uint64_t>(&fd)
            // );
        }

        master.cqeSeen(cqe);
        if(!master.prepAccept(
                acceptor.fd(),
                // (sockaddr*)&client_addr,
                // &client_addr_len,
                reinterpret_cast<uint64_t>(&acceptor)))
        {
            std::cerr << "failed to get SQE for accept\n";
            break;
        }
        master.submit();
    }
}