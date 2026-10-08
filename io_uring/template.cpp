#include <unistd.h>
#include <iostream>
#include <string>

#include <liburing.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ENTRIES 10

int main()
{
    io_uring ring;

    io_uring_queue_init(ENTRIES, &ring, 0);

    // 从 SQ 中获取一个空闲的 SQE，用来描述一次 I/O 操作。
    io_uring_sqe* sqe = io_uring_get_sqe(&ring);

    int fd;
    std::string ipbuf,opbuf;
    //常见IO操作
    //read
    /*
    void io_uring_prep_read(
        struct io_uring_sqe *sqe,
        int fd,
        void *buf,
        unsigned nbytes,
        __u64 offset
    );
    */
    io_uring_prep_read(
        sqe,
        fd,
        ipbuf.data(),
        4096,
        0
    );

    //write
    /*
    void io_uring_prep_write(
        struct io_uring_sqe *sqe,
        int fd,
        const void *buf,
        unsigned nbytes,
        __u64 offset
    );
    */
    io_uring_prep_write(
        sqe,
        fd,
        opbuf.data(),
        opbuf.length(),
        0
    );

    //recv
    /*
    void io_uring_prep_recv(
        struct io_uring_sqe *sqe,
        int sockfd,
        void *buf,
        unsigned len,
        unsigned flags
    );
    */
    io_uring_prep_recv(
        sqe,
        fd,
        ipbuf.data(),
        4096,
        0
    );

    //send
    /*
    void io_uring_prep_send(
        struct io_uring_sqe *sqe,
        int sockfd,
        void *buf,
        unsigned len,
        unsigned flags
    );
    */
    io_uring_prep_send(
        sqe,
        fd,
        opbuf.data(),
        opbuf.length(),
        0
    );

    //创建监听端口listen_fd
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    //允许端口重用
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //创建服务器地址
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //绑定端口和地址
    bind(listenfd, (sockaddr*)&server_addr, sizeof(server_addr));

    //将socket设为被动监听
    listen(listenfd, 1024);

    //accept
    /*
    void io_uring_prep_accept(
        struct io_uring_sqe *sqe,
        int fd,
        struct sockaddr *addr,
        socklen_t *addrlen,
        int flags
    );
    */

    io_uring_prep_accept(
        sqe,
        listenfd,
        nullptr,    // (sockaddr*)&server_addr,
        nullptr,    // sizeof(server_addr),
        SOCK_NONBLOCK
    );

    //提交用户定义的SQ（提交队列）给内核
    io_uring_submit(&ring);

    //等待至少一个 I/O 操作完成（阻塞）
    io_uring_cqe* cqe;
    /*
    int io_uring_wait_cqe(
        struct io_uring *ring,
        struct io_uring_cqe **cqe_ptr
    );
    */
    int ret = io_uring_wait_cqe(
        &ring,
        &cqe
    );
    /*
    返回值：
        0：成功获得CQE
        <0: 错误
    */
    if (ret < 0) {
        // error
    }

    //检查有无 I/O 操作完成（非阻塞）
    io_uring_cqe* cqe;
    /*
    int io_uring_peek_cqe(
        struct io_uring *ring,
        struct io_uring_cqe **cqe_ptr
    );
    */
    int ret = io_uring_peek_cqe(
        &ring,
        &cqe
    );

    if (ret == 0) {
        // 有完成事件
    }

    //cqe已完成，回收
    //CQ空间有限，及时腾出空位给其他IO
    /*
    void io_uring_cqe_seen(
        struct io_uring *ring,
        struct io_uring_cqe *cqe
    );
    */
    io_uring_cqe_seen(&ring, cqe);

    io_uring_queue_exit(&ring);
}