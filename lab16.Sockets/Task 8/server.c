#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1500
#define SERVER_PORT 12345

void run_server()
{
	int server;
	struct sockaddr_in server_addr, client_addr;
	socklen_t lenght = sizeof(struct sockaddr_in);
	char buffer[BUFFER_SIZE], *response = "Hello, from SERVER!";

	server = socket(AF_INET, SOCK_DGRAM, 0);

	if (server == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVER_PORT);

	if (bind(server, (struct sockaddr *)&server_addr, lenght) == -1)
	{
		perror("bind");
		close(server);
		exit(EXIT_FAILURE);
	}

	int bytes_recv = recvfrom(server, buffer,BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &lenght);
	if(bytes_recv <0){
		perror("recvfrom");
		close(server);
		exit(EXIT_FAILURE);
	}

	buffer[bytes_recv] = '\0';

	printf("Received from client on port: %d - %s\n", ntohs(client_addr.sin_port), buffer);

	int bytes_send = sendto(server, response, strlen(response), 0, (struct sockaddr*)&client_addr, lenght);
	if(bytes_send < 0){
		perror("sendto");
		close(server);
		exit(EXIT_FAILURE);
	}

	close(server);
}

int main()
{
	run_server();
	return 0;
}