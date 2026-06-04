#include <iostream>
#include <string_view>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

constexpr string_view SERVER_IP = "127.0.0.1";
constexpr int PORT = 9527;
constexpr int BACKLOG = 4;


int main()
{
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    cout<<serverfd<<endl;

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP.data(), &server_addr.sin_addr.s_addr);


    bind(serverfd, (const sockaddr*)&server_addr, sizeof(server_addr));

    listen(serverfd, BACKLOG);

    while(1)
    {
        sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int clientfd = accept(serverfd, (sockaddr*)&client_addr, &len); 

        char buffer[4096] = {0};
        read(clientfd, buffer, 4096);

        cout<<  buffer  <<endl;

        const char msg[4096] = "successful connect!";
        write(clientfd, msg, sizeof(msg));

        close(clientfd);
    }
    
}