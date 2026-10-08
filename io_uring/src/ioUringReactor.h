#pragma once

#include <unordered_map>
#include <memory>

#include "ioUring.h"
#include "bufferPool.h"

class Connection;
class IoUringReactor
{
    private:
        int no;
        int capcity;
        IoUring iu{};
        BufferPool bufPool;
        std::unordered_map<int, std::shared_ptr<Connection>> connections;

    public:
        IoUringReactor() = default;
        IoUringReactor(int maxEvents, int idx);
        ~IoUringReactor();
        
        bool push(int fd);
        bool pop(int fd);

        void workloop();
};