#include <iostream>

#include "connection.h"

// //--------------------------------------------Buffer类--------------------------------------------//
// size_t Buffer::size()
// {
//     return buffer.length();
// }
          
// bool Buffer::empty()
// {
//     return buffer.length()==0;
// }

// void Buffer::append(const char* s, size_t len)
// {
//     buffer.append(s, len);
// }

// void Buffer::append(const std::string& s)
// {
//     buffer.append(s);
// }

// void Buffer::retrieve(size_t len)
// {
//     buffer.erase(0, len);
// }
        
// void Buffer::retrieveAll()
// {
//     buffer.clear();
// }

// std::string& Buffer::data()
// {
//     return buffer;
// }

//--------------------------------------------Connection类--------------------------------------------//
Connection::Connection(int _fd):fd(_fd),connected(true){};

// void Connection::handleRead(Reactor* reactor)
// {
    
// }

// void Connection::handleWrite(Reactor* reactor)
// {
    
// }

Connection::~Connection()
{
    close(fd);
}