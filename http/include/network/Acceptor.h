#ifndef ACCOPTOR_H
#define ACCOPTOR_H

constexpr int backlog = 1024;

class Acceptor
{
    private:
        int listenfd;

    public:
        Acceptor(int port);

        int fd() const;

        int acceptConnection();

        ~Acceptor();
};


#endif