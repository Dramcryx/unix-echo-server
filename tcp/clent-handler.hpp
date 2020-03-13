#ifndef CLIENT_HANDLER
#define CLIENT_HANDLER

#include <thread>
#include <iostream>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>

class client_handler
{
public:
    client_handler(int client_fd):
        new_connection(client_fd),
        trd(&client_handler::infiniteloop, this)
    {}
private:
    int new_connection;
    std::thread trd;

    void infiniteloop()
    {
        char buffer[1024] = {0};
        int valread = 0;
        while (std::strcmp(buffer, "stop"))
        {
            std::memset(buffer, 0, 1024);
            valread = read( new_connection , buffer, 1024);
            if (valread < 0)
            {
                perror("fucked reading up");
                exit(EXIT_FAILURE);
            }
            if (valread == 0)
            {
                break;
            }
            std::cout << "FROM CLIENT " << new_connection << ": " << buffer << "\n";
            if (send(new_connection , buffer , strlen(buffer) , 0) < 0)
            {
                perror("sendback error");
                exit(EXIT_FAILURE);
            }
        }
        close(new_connection);
        std::cout << "Client quit: " << new_connection << "\n";
    }
};

#endif