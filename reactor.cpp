#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

constexpr int PORT = 8888;
constexpr int MAX_CLIENTS = 30;

int main()
{
    int client_sockets[MAX_CLIENTS];
    int max_fd;
    char buffer[1024];

    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    // 初始化所有客户端套接字为 0
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    //创建套接字socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    //
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    bind(listen_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    listen(listen_fd, 5);

    fd_set read_fds;    



    while(1)
    {
        FD_ZERO(&read_fds);
        FD_SET(listen_fd, &read_fds);

        max_fd = listen_fd;
        cout<<listen_fd<<endl;
        for (int i = 0; i < MAX_CLIENTS; i++) {     //将所有连接中的端口加入到读集合中,用于在select处判断客户端是否可读
            int sd = client_sockets[i];
            if (sd > 0) {
                FD_SET(sd, &read_fds);
                if (sd > max_fd) max_fd = sd;
            }
        }

        int activity = select(max_fd+1, &read_fds, NULL, NULL, NULL);

        if(FD_ISSET(listen_fd, &read_fds))          //监听新连接
        {
            int client_fd = accept(listen_fd, (sockaddr*)&client_addr, &len);

            if(client_fd < 0)
            {
                continue;
            }


            for(int i=0;i < MAX_CLIENTS;i++)
            {
                if(client_sockets[i]==0)
                {
                    client_sockets[i] = client_fd;
                    break;
                }
            }
        }

        for(int i=0;i < MAX_CLIENTS;i++)
        {
            int sd = client_sockets[i];
            if(FD_ISSET(sd,&read_fds))
            {
                int n = read(sd, buffer, sizeof(buffer)-1);
                if(n==0)
                {
                    cout<<"Client: "<<sd<<" disconnected!"<<endl;
                    close(sd);
                    client_sockets[i] = 0;
                }else if(n < 0)
                {
                    cout<<"read error!"<<endl;
                    close(sd);
                    client_sockets[i] = 0;
                }else
                {
                    buffer[n] = '\0';
                    write(sd, buffer, n);
                }
            }
        }
    }

}