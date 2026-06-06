#include "Connection.h"




Connection::Connection(int _fd):fd(_fd),connected(true){};


void Connection::handleRead()
{
    //处理对应连接的写入
}

void Connection::handleWrite()
{
    //处理对应连接的写入
}