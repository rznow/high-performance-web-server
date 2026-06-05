#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
using namespace std;

constexpr int PORT = 8888;
constexpr int MAX_CLIENTS = 30;

int main()
{
    // int client_sockets[MAX_CLIENTS];
    // 初始化所有客户端套接字为 0
    // for (int i = 0; i < MAX_CLIENTS; i++) {
    //     client_sockets[i] = 0;
    // }

    vector<int> client_sockets;
    client_sockets.reserve(30);
    int max_fd;
    char buffer[1024];

    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    //创建套接字socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    //创建服务端地址
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // inet_pton(AF_INET, INADDR_ANY, &server_addr.sin_addr);

    bind(listen_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    listen(listen_fd, 5);

    fd_set read_fds;    

    while(1)
    {
        FD_ZERO(&read_fds);
        FD_SET(listen_fd, &read_fds);

        max_fd = listen_fd;

        // for (int i = 0; i < MAX_CLIENTS; i++) {     //将所有连接中的端口加入到读集合中,用于在select处判断客户端是否可读
        //     int sd = client_sockets[i];
        //     if (sd > 0) {
        //         FD_SET(sd, &read_fds);
        //         if (sd > max_fd) max_fd = sd;
        //     }
        // }
        for(auto &sd : client_sockets)
        {
            FD_SET(sd, &read_fds);
            max_fd = max(max_fd, sd);
        }

        int activity = select(max_fd+1, &read_fds, NULL, NULL, NULL);

        if(FD_ISSET(listen_fd, &read_fds))          //监听新连接
        {
            int client_fd = accept(listen_fd, (sockaddr*)&client_addr, &len);

            if(client_fd < 0)
            {
                perror(" connect失败! ");
                break;
            }

            client_sockets.emplace_back(client_fd);

            if(client_sockets.size() > MAX_CLIENTS)
            {
                perror("端口数超出容量!");
                exit(1);
            }
            // for(int i=0;i < MAX_CLIENTS;i++)
            // {
            //     if(client_sockets[i]==0)
            //     {
            //         client_sockets[i] = client_fd;
            //         break;
            //     }
            // }
        }


        for(auto it = client_sockets.begin();it != client_sockets.end();)
        {
            int sd = *it;
            if(FD_ISSET(sd, &read_fds)) //读就绪
            {
                int n = read(sd, buffer, sizeof(buffer)-1);
                if(n == 0)
                {
                    cout<<"Client: "<<sd<<" disconnected!"<<endl;
                    close(sd);
                    it = client_sockets.erase(it);
                }else if(n < 0)
                {
                    cout<<"read error!"<<endl;
                    close(sd);
                    it = client_sockets.erase(it);
                }else
                {
                    buffer[n] = '\0';
                    write(sd, buffer, n);
                    it++;
                }
            }else   it++;
        }

        // for(int i=0;i < MAX_CLIENTS;i++)
        // {
        //     int sd = client_sockets[i];
        //     if(FD_ISSET(sd,&read_fds))
        //     {
        //         int n = read(sd, buffer, sizeof(buffer)-1);
        //         if(n==0)
        //         {
        //             cout<<"Client: "<<sd<<" disconnected!"<<endl;
        //             close(sd);
        //             client_sockets[i] = 0;
        //         }else if(n < 0)
        //         {
        //             cout<<"read error!"<<endl;
        //             close(sd);
        //             client_sockets[i] = 0;
        //         }else
        //         {
        //             buffer[n] = '\0';
        //             write(sd, buffer, n);
        //         }
        //     }
        // }

    }
    return 0;
}