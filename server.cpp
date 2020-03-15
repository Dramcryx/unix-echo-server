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
#include <set>
#include <algorithm>

#include "socket.hpp"
#include "client-handler.hpp"

#define PORT 8080

template <typename Iterator>
bool find_equal(Iterator begin, Iterator end, sockaddr_in val)
{
    while (begin != end)
    {
        if ((begin->sin_addr.s_addr == val.sin_addr.s_addr) && (begin->sin_port == val.sin_port))
        {
            return true;
        }
        ++begin;
    }
    return false;
}


int main(int argc, char const *argv[]) 
{
	sockaddr_in address; 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 

    TCPSocket server_fd(address);
    UDPSocket udp(address);
    
    std::map<int, std::thread> clients;
    using cr_kv_t = const std::map<int, std::thread>::value_type &;

    std::vector<sockaddr_in> udp_clients;

    while (true)
    {
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(server_fd, &readset);
        FD_SET(udp, &readset);

        int max = std::max(server_fd.toInt(), udp.toInt());

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
        if (FD_ISSET(udp, &readset))
        {
            sockaddr_in cliaddr;
            memset(&cliaddr, 0, sizeof(cliaddr));
            unsigned len = sizeof(sockaddr_in); //len is value/resuslt
            char buffer[1024]{0};
            
            int recv_count = 0;
            if ((recv_count = recvfrom(udp, (char *)buffer, 1024, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len)) < 0)
            {
                perror("Receive failure");
                exit(EXIT_FAILURE);
            }
            if (!find_equal(udp_clients.begin(), udp_clients.end(), cliaddr))
            {
                udp_clients.push_back(cliaddr);
                std::cout << "Новый UDP клиент: " << cliaddr.sin_addr.s_addr << "\n";
            }
            std::thread(udp_runner, udp, cliaddr, buffer).detach();
        }
        //todo notify about close clients
    }
	return 0; 
} 

