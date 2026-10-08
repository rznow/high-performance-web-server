#pragma once

#include <liburing.h>

class IoUring
{
public:
    explicit IoUring(unsigned entries = 2);
    ~IoUring();

    IoUring(const IoUring&) = delete;
    IoUring& operator=(const IoUring&) = delete;

    IoUring(IoUring&&) noexcept;
    IoUring& operator=(IoUring&&) noexcept;

    io_uring_sqe* getSqe();

    io_uring* get();

    bool read(
        int fd,
        void* buf,
        unsigned len,
        __u64 offset,
        uint64_t userData);

    bool readFixed(
        int fd,
        void* buf,
        unsigned len,
        __u64 offset,
        size_t idx,
        uint64_t userData);

    bool write(
        int fd,
        void* buf,
        unsigned len,
        __u64 offset,
        uint64_t userData);

    bool recv(
        int fd,
        void* buf,
        unsigned len,
        int flags,
        uint64_t userData);

    bool send(
        int fd,
        const void* buf,
        unsigned len,
        int flags,
        uint64_t userData);

    bool prepAccept(
        int listenFd,
        // sockaddr* addr,
        // socklen_t* addrLen,
        uint64_t userData);

    int submit();

    int waitCqe(io_uring_cqe** cqe);

    int peekCqe(io_uring_cqe** cqe);

    void cqeSeen(io_uring_cqe* cqe);

private:
    io_uring ring{};
};