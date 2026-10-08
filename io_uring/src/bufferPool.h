#pragma once
#include <cstddef>
#include <liburing.h>

class BufferPool
{
    private:
        char* memory_;

        size_t bufferSize_;
        size_t bufferCount_;

        size_t* next;
        size_t free_head;
    public:
        BufferPool(io_uring* ring, size_t bufferSize, size_t bufferCount);
        BufferPool(const BufferPool&) = delete;
        BufferPool& operator=(const BufferPool&) = delete;
        ~BufferPool();

        void* get(size_t index);

        bool acquire(size_t& index);
        bool release(size_t index);
};