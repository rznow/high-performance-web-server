#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <unistd.h>
#include <memory>
using namespace std;


class Reactor;
class ThreadPool;
class Buffer
{
    private:
        string buffer;
    public:
        size_t size();
        bool empty();
        void append(const char* s, size_t len);
        void append(const std::string& s);
        void retrieve(size_t len);
        void retrieveAll();
        const std::string& data() const;
};

class Connection: public std::enable_shared_from_this<Connection>
{
    private:
        int fd;
        bool connected;
        Buffer inputbuffer;
        Buffer outputbuffer;
        ThreadPool *pool;
    public:
        Connection(int _fd, ThreadPool* _pool);

        void handleRead(Reactor* reactor);

        void handleWrite(Reactor* reactor);

        ~Connection();
};

#endif