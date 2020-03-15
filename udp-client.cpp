#include <string>
#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080

int main()
{
	int sockfd;
	char buffer[1024];
	sockaddr_in	 servaddr;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;
	
	unsigned n, len;
	

    ssize_t recv_count = 0;
    ssize_t send_count = 0;

    timeval timeout{2, 0};
    std::cout << "Введите \'stop\', чтобы остановить работу \n";
    while (1)
    {
        std::cout << "Введите сообщение: ";
        std::string message;
        std::getline(std::cin, message);
        if (!message.length())
        {
            continue;
        }
        if (!(std::strcmp(message.c_str(), "stop")))
        {
            break;
        }

        resend:
	    if ((send_count = sendto(sockfd, message.c_str(), message.size(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)
        {
            perror("Send failure");
            exit(EXIT_FAILURE);
        }
        std::cout << "sendcount " << send_count << "\n";
        if (!send_count)
        {
            std::cout << "Сегодня без посылочек обойдёмся\n";
            break;
        }

        fd_set set;
        FD_ZERO(&set);
        FD_SET(sockfd, &set);

        select(sockfd + 1, &set, 0, 0, &timeout);
        
        if (FD_ISSET(sockfd, &set))
        {
	        if ((n = recvfrom(sockfd, buffer, 1024, MSG_WAITALL, (sockaddr *) &servaddr, &len)) < 0)
            {
                perror("Receive failure");
                exit(EXIT_FAILURE);
            }
	        buffer[n] = '\0';
	        printf("От сервера : %s\n", buffer);
        }
        else
        {
            std::cout << "Нет ответа от сервера. Повторить отправку? y/n: ";
            char c;
            std::cin >> c;
            if (c == 'y')
            {
                goto resend;
            }
            else if (c == 'n')
            {
                continue;
            }
        }
    }
	close(sockfd); 
	return 0; 
} 
