#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
using namespace std;

class Connection{
    private:
        int fd;
        bool connected;
        string inputbuffer;
        string outputbuffer;
    public:
        Connection(int _fd);

        void handleRead();

        void handleWrite();
};

#endif