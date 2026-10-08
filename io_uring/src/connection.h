#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <unistd.h>
#include <memory>


class ThreadPool;
// class Buffer
// {
//     private:
//         std::string buffer;
//     public:
//         size_t size();
//         bool empty();
//         void append(const char* s, size_t len);
//         void append(const std::string& s);
//         void retrieve(size_t len);
//         void retrieveAll();
//         std::string& data();
// };

class Connection: public std::enable_shared_from_this<Connection>
{
    private:
        int fd;
        bool connected;
        size_t ipBuf_idx;
        size_t opBuf_idx;
        // ThreadPool *pool;
    public:
        Connection(int _fd);

        // void handleRead(Reactor* reactor);

        // void handleWrite(Reactor* reactor);

        ~Connection();

        friend class IoUringReactor;
};

#endif