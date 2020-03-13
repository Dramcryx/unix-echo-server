#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <stdexcept>

#include <cstdio>

#include <netinet/in.h>
#include <sys/socket.h>

class TCPSocket
{
public:
    TCPSocket(const sockaddr_in & address)
    {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	    {
            throw std::runtime_error("Ошибка открытия TCP сокета");
	    }

        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            throw std::runtime_error("Ошибка установки опций сокета");
        }

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	    {
		    throw std::runtime_error("Ошибка привязки к адресу");
        }

        if (listen(server_fd, 100) < 0) 
        {
            throw std::runtime_error("Ошибка задания количества слушателей");
        }
    }

    int socket_desc() const
    {
        return server_fd;
    }

    inline operator int() const
    {
        return server_fd;
    }

private:
    int server_fd;
};

class UDPSocket
{
public:
    UDPSocket(const sockaddr_in & servaddr)
    {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
		    throw std::runtime_error("Ошибка открытия UDP сокета");
	    }
        if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	    {
            throw std::runtime_error("Ошибка привязки к адресу");
	    }
    }

    inline operator int() const
    {
        return sockfd;
    }
private:
    int sockfd;
};

#endif