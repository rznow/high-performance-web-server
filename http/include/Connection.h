#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <unistd.h>
using namespace std;


class Reactor;
class Connection{
    private:
        int fd;
        bool connected;
        string inputbuffer;
        string outputbuffer;
    public:
        Connection(int _fd);

        void handleRead(Reactor* reactor);

        void handleWrite();

        ~Connection();
};

#endif