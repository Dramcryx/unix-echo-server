#include <unistd.h> 
#include <cstdio> 
#include <sys/socket.h> 
#include <cstdlib> 
#include <netinet/in.h>
#include <fcntl.h>

#include <cstring>

#include <map>
#include <vector>
#include <iostream>
#include <thread>
#include <algorithm>

#include "../socket.hpp"
#include "clent-handler.hpp"

#define PORT 8080
int main(int argc, char const *argv[]) 
{
	sockaddr_in address; 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 

    TCPSocket server_fd(address);
    
    std::map<int, std::thread> clients;
    using cr_kv_t = const std::map<int, std::thread>::value_type &;

    timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    while (true)
    {
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(server_fd, &readset);
        for (auto &i: clients)
        {
            if (i.second.joinable())
            {
                FD_SET(i.first, &readset);
            }
        }
        
        int max = server_fd;
        if (clients.size())
        {
            max = std::max(server_fd.socket_desc(), std::max_element(clients.begin(), clients.end(), [&](cr_kv_t l,
                                                                                                         cr_kv_t & r){
                            return l.first > r.first;
                        })->first);
        }

        if(select(max+1, &readset, NULL, NULL, NULL) <= 0)
        {
            perror("select");
            exit(3);
        }

        // Определяем тип события и выполняем соответствующие действия
        if(FD_ISSET(server_fd, &readset))
        {
            // Поступил новый запрос на соединение, используем accept
            int sock = accept(server_fd, NULL, NULL);
            if(sock < 0)
            {
                perror("accept");
                exit(3);
            }
            std::cout << "Новый TCP клиент: " << sock << "\n";

            clients[sock] = std::thread(tcp_runner, sock);
        }
        //todo notify about close clients
    }
	return 0; 
} 

