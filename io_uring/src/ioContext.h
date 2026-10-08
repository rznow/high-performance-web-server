#pragma once
#include <memory>

class Connection;
enum class IoType
{
    READ,
    WRITE,
    RECV,
    SEND,
    ACCEPT
};

struct IoContext
{
    IoType type;
    std::shared_ptr<Connection> conn;
};