#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <unistd.h>
using namespace std;


class Reactor;

class Buffer
{
    private:
        string buffer;
    public:
        size_t size();
        bool empty();
        void append(string s);
        void retrieve(size_t len);
        void retrieveAll();
        const std::string& data() const;
};

class Connection
{
    private:
        int fd;
        bool connected;
        Buffer inputbuffer;
        Buffer outputbuffer;
    public:
        Connection(int _fd);

        void handleRead(Reactor* reactor);

        void handleWrite();

        ~Connection();
};

#endif