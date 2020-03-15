#ifndef CLIENT_HANDLER
#define CLIENT_HANDLER

#include <thread>
#include <iostream>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>

void tcp_runner(int new_connection)
{
    char buffer[1024] = {0};
    int valread = 0;
    while (std::strcmp(buffer, "stop"))
    {
        std::memset(buffer, 0, 1024);
        valread = read( new_connection , buffer, 1024);
        if (valread < 0)
        {
            perror("Ошибка чтения");
            exit(EXIT_FAILURE);
        }
        if (valread == 0)
        {
            break;
        }
        std::cout << "TCP клиент " << new_connection << ": " << buffer << "\n";
        if (send(new_connection , buffer , strlen(buffer) , 0) < 0)
        {
            perror("sendback error");
            exit(EXIT_FAILURE);
        }
    }
    std::cout << "TCP клиент вышел: " << new_connection << "\n";
}

void udp_runner(int sockfd, const sockaddr_in & cliaddr, const std::string & buffer)
{
    std::cout << "UDP клиент " << cliaddr.sin_addr.s_addr << ": " << buffer << "\n";
    int send_count = 0;
    while (!send_count)
    {
        if ((send_count = sendto(sockfd, buffer.c_str(), buffer.length(), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(sockaddr_in))) < 0)
        {
            perror("Send failure");
            exit(EXIT_FAILURE);
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    }
}

#endif