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

    int opt = 1; 
	int addrlen = sizeof(address);
	// Creating socket file descriptor 
	// if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	// { 
	// 	perror("socket failed"); 
	// 	exit(EXIT_FAILURE); 
	// } 
	
	// Forcefully attaching socket to the port 8080 
	// if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	// { 
	//  	perror("setsockopt"); 
	//  	exit(EXIT_FAILURE); 
	// } 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port 8080 
	// if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
	// { 
	// 	perror("bind failed"); 
	// 	exit(EXIT_FAILURE); 
	// }

    // if (listen(server_fd, 100) < 0) 
    // { 
    //     perror("listen"); 
    //     exit(EXIT_FAILURE); 
    // }

    TCPSocket server_fd(address);
    
    std::map<int, std::thread> clients;
    std::vector<client_handler> vec;

    while (true)
    {
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(server_fd, &readset);
        for (auto &i: clients)
        {
            FD_SET(i.first, &readset);
        }

        timeval timeout;
        timeout.tv_sec = 500;
        timeout.tv_usec = 0;
        
        int max = std::max(server_fd.socket_desc(), std::max_element(clients.begin(), clients.end(), [&](const std::pair<const int, std::thread> & l,
                                                                                           const std::pair<const int, std::thread> & r) -> bool{
            return l.first > r.first;
        })->first);

        if(select(max+1, &readset, NULL, NULL, &timeout) <= 0)
        {

            // TODO check stop word
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
            std::cout << "new connection: " << sock << "\n";
            //fcntl(sock, F_SETFL, O_NONBLOCK);

            vec.emplace_back(sock);
            
            //clients[sock] = std::move(std::thread(infiniteloop, sock));
        }
        auto closed_client = std::find_if(clients.begin(), clients.end(), [](const std::pair<const int, std::thread> & l)
        {
            return !l.second.joinable();
        });
        if (closed_client != clients.end())
        {
            clients.erase(closed_client);
            std::cout << "Clients remaining: " << clients.size() << "\n";
        }
    }
    for (auto &i: clients)
    {
        close(i.first);
        i.second.join();
    }
	return 0; 
} 

