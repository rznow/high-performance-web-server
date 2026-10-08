#include <iostream>
#include <cstring>

#include "bufferPool.h"


BufferPool::BufferPool(io_uring* ring, size_t bufferSize, size_t bufferCount)
    :bufferSize_(bufferSize),
    bufferCount_(bufferCount)
{
    memory_ = new char[bufferSize_*bufferCount_];

    next = new size_t[bufferCount_];

    iovec* iovecs = new iovec[bufferCount_];

    for(size_t i=0;i < bufferCount_;i++)
    {
        iovecs[i].iov_base  = get(i);
        iovecs[i].iov_len   = bufferSize_;
    }

    int ret = io_uring_register_buffers(
        ring,
        iovecs,
        bufferCount_
    );

    if (ret < 0)
    {
        std::cerr << "register buffers failed: "
            << strerror(-ret)
            << " (" << -ret << ")\n";
    }
    else
    {
        std::cout << "register buffers success\n";
    }

    delete[] iovecs;

    for(size_t i = 0;i < bufferCount_-1;i++)
    {
        next[i] = i+1;
    } 

    next[bufferCount_-1] = SIZE_MAX;

    free_head = 0;
}

BufferPool::~BufferPool()
{
    delete[] memory_;
}

void* BufferPool::get(size_t index)
{
    if(index >= bufferCount_)
        return nullptr;

    return memory_ + index * bufferSize_;
}

bool BufferPool::acquire(size_t& index)
{
    if(free_head == SIZE_MAX)
        return false;
    
    size_t i = free_head;
    free_head = next[i];

    index = i;

    return true;
}

bool BufferPool::release(size_t index)
{
    if(index < 0||index >= bufferCount_)
        return false;
    
    next[index] = free_head;
    free_head = index;

    return true;
}