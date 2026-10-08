#include <iostream>
#include <cstring>
#include "ioUringReactor.h"
#include "connection.h"
#include "ioContext.h"



// IoUringReactor::IoUringReactor()
// {

// }

IoUringReactor::IoUringReactor(int maxEvents, int idx): no(idx), capcity(maxEvents), bufPool(iu.get(), 4096, 512){}

IoUringReactor::~IoUringReactor()
{
    
}

bool IoUringReactor::push(int fd)
{
    auto conn = std::make_shared<Connection>(fd);
    
    bufPool.acquire(conn->ipBuf_idx);
    bufPool.acquire(conn->opBuf_idx);
    // std::cout<<"fd:\t"<<fd<<std::endl;
    // std::cout<<"conn:\t"<<conn.get()<<std::endl;

    // if(iu.recv(
    //     fd,
    //     bufPool.get(conn->ipBuf_idx),
    //     4096,
    //     0,
    //     reinterpret_cast<uint64_t>(conn.get())))

    IoContext* readIC = new IoContext(
        IoType::READ,
        conn
    );

    if(iu.readFixed(
        fd,
        bufPool.get(conn->ipBuf_idx),
        4096,
        0,
        conn->ipBuf_idx,
        reinterpret_cast<uint64_t>(readIC)))
    {
        connections[fd] = std::move(conn);
        iu.submit();
    }else   return false;

    return true;
}

bool IoUringReactor::pop(int fd)
{
    if(connections.find(fd) == connections.end())
        return false;

    auto conn = connections[fd];
    bufPool.release(conn->ipBuf_idx);
    bufPool.release(conn->opBuf_idx);

    connections.erase(fd);

    return true;
}

void IoUringReactor::workloop()
{
    io_uring_cqe* cqe;
    while(true)
    {
        if(iu.waitCqe(&cqe) < 0)
            continue;
        
        int res = cqe->res;
        auto IC = reinterpret_cast<IoContext*>(cqe->user_data);
        auto conn = IC->conn;
        int fd = conn->fd;
        iu.cqeSeen(cqe);

        if(IC->type != IoType::READ) 
        {
            delete IC;
            continue;
        }

        if(res > 0)
        {
            std::string s((char*)bufPool.get(conn->ipBuf_idx), res);

            std::cout<<"fd:\t\t"<<fd<<std::endl;
            std::cout<<"conn:\t\t"<<conn<<std::endl;
            std::cout<<"read:\t\t"<<res<<std::endl;
            std::cout<<"input_idx:\t"<<conn->ipBuf_idx<<std::endl;
            std::cout<<"output_idx:\t"<<conn->opBuf_idx<<std::endl;
            std::cout<<"read buffer:\t"<<s<<std::endl;

            /*

                处理来自客户端的请求（s）
            
            */

            std::string msg = "message from server!";

            IoContext* writeIC = new IoContext(
                IoType::WRITE,
                conn
            );

            iu.write(
                fd,
                msg.data(),
                msg.length(),
                0,
                reinterpret_cast<uint64_t>(writeIC));

            // iu.recv(
            //     fd,
            //     bufPool.get(conn->ipBuf_idx),
            //     4096,
            //     0,
            //     reinterpret_cast<uint64_t>(conn));
            iu.readFixed(
                fd,
                bufPool.get(conn->ipBuf_idx),
                4096,
                0,
                conn->ipBuf_idx,
                reinterpret_cast<uint64_t>(IC));

            iu.submit();
        }else if(res == 0)//关闭连接
        {
            pop(fd);
            std::cout<<"关闭连接\t"<<"fd:"<<fd<<std::endl<<std::endl;

        }else if(res < 0)//
        {
            pop(fd);
            std::cerr << "recv failed: " << strerror(-res) << std::endl;   
        }

    }


}