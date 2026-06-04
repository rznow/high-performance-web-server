#include <iostream>
#include <string_view>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;
/*
    <unistd.h>：POSIX 操作系统 API 头文件，包含 read、write、close、socket 相关的函数声明（如 close、read），以及 fork、exec 等系统调用。
    <arpa/inet.h>：网络编程头文件，包含 IP 地址转换函数（如 inet_addr、inet_ntoa、inet_pton、inet_ntop），以及 htons、htonl、ntohs、ntohl 等字节序转换函数。该头文件还定义了 sockaddr_in 结构体等。
*/

constexpr string_view SERVER_IP = "127.0.0.1";
constexpr int PORT = 9527;

int main()
{
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1)
    {
        cerr << "socket 创建失败!"<<endl;
    }
    cout<< clientfd <<endl;
    
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);                 // 服务器端口
    if (inet_pton(AF_INET, SERVER_IP.data(), &server_addr.sin_addr.s_addr) != 1) {
        cerr << "无效的 IP 地址" << endl;
        close(clientfd);
        return 1;
    }
    // server_addr.sin_addr.s_addr = inet_addr(SERVER_IP.data()); // 服务器IP（本机）


    if(connect(clientfd, (const sockaddr*)&server_addr, sizeof(server_addr))==-1)
    {
        cerr << "connect 失败!" << endl;
    }

    const char* msg = "Connect to Server!";
    if (send(clientfd, msg, strlen(msg), 0) == -1) {
        close(clientfd);
        return 1;
    }
    cout << "发送: " << msg << endl;

    char buffer[4096] = {0};
    int n = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
    if (n == -1) {
        perror("recv");
    } else if (n == 0) {
        cout << "服务器关闭了连接" << endl;
    } else {
        cout << "收到: " << buffer << endl;
    }

    close(clientfd);
    return 0;

}