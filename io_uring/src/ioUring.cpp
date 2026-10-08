// #include <arpa/inet.h>
#include "ioUring.h"

IoUring::IoUring(unsigned entries)
{
    io_uring_queue_init(entries, &ring, 0);
}

IoUring::~IoUring()
{
    io_uring_queue_exit(&ring);
}

io_uring_sqe* IoUring::getSqe()
{
    return io_uring_get_sqe(&ring);
}

io_uring* IoUring::get()
{
    return &ring;
}

bool IoUring::read(
    int fd,
    void* buf,
    unsigned len,
    __u64 offset,
    uint64_t userData)
{
    io_uring_sqe* sqe = getSqe();

    if(!sqe)
    {
        return false;
    }

    io_uring_prep_read(
        sqe,
        fd,
        buf,
        len,
        offset
    );

    sqe->user_data = userData;

    return true;
}

bool IoUring::readFixed(
    int fd,
    void* buf,
    unsigned len,
    __u64 offset,
    size_t idx,
    uint64_t userData)
{
    io_uring_sqe* sqe = getSqe();

    if(!sqe)
    {
        return false;
    }

    io_uring_prep_read_fixed(
        sqe,
        fd,
        buf,
        len,
        offset,
        idx
    );

    sqe->user_data = userData;

    return true;
}

bool IoUring::write(
    int fd,
    void* buf,
    unsigned len,
    __u64 offset,
    uint64_t userData)
{
    io_uring_sqe* sqe = getSqe();

    if(!sqe)
    {
        return false;
    }

    io_uring_prep_write(
        sqe,
        fd,
        buf,
        len,
        offset
    );

    sqe->user_data = userData;

    return true;
}


bool IoUring::recv(
    int fd,
    void* buf,
    unsigned len,
    int flags,
    uint64_t userData)
{
    io_uring_sqe* sqe = getSqe();

    if(!sqe)
    {
        return false;
    }

    io_uring_prep_recv(
        sqe,
        fd,
        buf,
        len,
        flags
    );

    sqe->user_data = userData;

    return true;
}

bool IoUring::send(
    int fd,
    const void* buf,
    unsigned len,
    int flags,
    uint64_t userData)
{
    io_uring_sqe* sqe = getSqe();

    if(!sqe)
    {
        return false;
    }

    io_uring_prep_send(
        sqe,
        fd,
        buf,
        len,
        flags
    );

    sqe->user_data = userData;

    return true;
}

bool IoUring::prepAccept(
    int listenFd,
    // sockaddr* addr,
    // socklen_t* addrLen,
    uint64_t userData)
{
    io_uring_sqe* sqe = getSqe();

    if (sqe == nullptr)
    {
        return false;
    }

    // sockaddr_in client_addr{};
    // socklen_t client_addr_len = sizeof(client_addr);

    io_uring_prep_accept(
        sqe,
        listenFd,
        nullptr,// (sockaddr*)&client_addr,
        nullptr,// &client_addr_len,
        0
    );

    sqe->user_data = userData;

    return true;
}


/*
    返回值：
        >0: 提交SQE的个数
        0： 没有提交SQE
        <0: 提交失败
*/
int IoUring::submit()
{
    return io_uring_submit(&ring);
}

/*
    返回值：
        0： 成功获得CQE
        <0: 错误
*/
int IoUring::waitCqe(io_uring_cqe** cqe)
{
    return io_uring_wait_cqe(&ring, cqe);
}

/*
    返回值：
        0： 有CQE
        <0: 错误
*/
int IoUring::peekCqe(io_uring_cqe** cqe)
{
    return io_uring_peek_cqe(&ring, cqe);
}

void IoUring::cqeSeen(io_uring_cqe* cqe)
{
    io_uring_cqe_seen(&ring, cqe);
}