#include <iostream>

#include <cstdio>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[]) 
{ 
	int sock = 0, valread; 
	sockaddr_in serv_addr;
	std::string input;
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		printf("Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("Invalid address/ Address not supported \n"); 
		return -1; 
	} 

    if (connect(sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("Connection Failed\n"); 
		return -1; 
	}


	std::cout << "Чтобы остановить работу, отправьте \'stop\'\n";
	ssize_t bytes_sent = 0;

    while (1)
    {
		std::cout << "Сообщение серверу: ";
		std::getline(std::cin, input);
        
		if (input.length())
		{
			if (!(std::strcmp(input.c_str(), "stop")))
        	{
            	break;
        	}
			std::cout << "Шлём серверу: " << input << "\n";
	    	if ((bytes_sent = send(sock , input.c_str(), input.length() + 1, 0)) < 0)
        	{
            	perror("Ошибка отправки");
            	exit(EXIT_FAILURE);
        	}
			if (bytes_sent == 0)
			{
				std::cout << "Сервер упал, пока вы писали сообщение\n";
				break;
			}
        	input.clear();

			if ((valread = read(sock, buffer, 1024)) < 0)
			{
				perror("Ошибка приёма");
				exit(EXIT_FAILURE);
			}
			if (valread == 0)
			{
				std::cout << "Сервер ничего не записал\n";
				break;
			} 
        	std::cout << "Возврат от сервера: " << buffer << "\n";
        	std::memset(buffer, '\0', 1024);
		}
    }
	std::cout << "Закрываем соединение...\n";
    shutdown(sock, SHUT_RDWR);
    close(sock);
	return 0; 
} 

