#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <time.h>

#define PORT_TCP 7777
#define PORT_UDP 7778
#define BUFFER_SIZE 1024
int socket_tcp, socket_udp;

void run_server();
int init_tcp();
int init_udp();
void handle_tcp(int socket_tcp, char *response);
void handle_udp(int socket_udp, char *response);
void sigint_handler(int sig);

int main()
{
	run_server();
	close(socket_tcp);
	close(socket_udp);

	return 0;
}

void sigint_handler(int sig)
{
	printf("Exiting...\n");
	exit(EXIT_SUCCESS);
}

void run_server()
{
	signal(SIGINT, sigint_handler);

	fd_set readyfds;
	int select_result, max_fd;
	char response[BUFFER_SIZE];

	socket_tcp = init_tcp();
	socket_udp = init_udp();

	max_fd = ((socket_tcp > socket_udp) ? socket_tcp : socket_udp) + 1;

	time_t mytime;
	struct tm *now;

	printf("\tSELECT\nTCP server on port: %d\nUDP server of port: %d\n", PORT_TCP, PORT_UDP);

	while (1)
	{
		FD_ZERO(&readyfds);
		FD_SET(socket_tcp, &readyfds);
		FD_SET(socket_udp, &readyfds);

		select_result = select(max_fd, &readyfds, NULL, NULL, NULL);

		mytime = time(NULL);
		now = localtime(&mytime);

		sprintf(response, "Date: %d.%d.%d\tTime: %d:%d:%d", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec);

		if (select_result == -1)
		{
			perror("select");
			exit(EXIT_FAILURE);
		}
		if (FD_ISSET(socket_tcp, &readyfds))
		{
			printf("TCP server got a message\n");
			handle_tcp(socket_tcp, response);
		}
		if (FD_ISSET(socket_udp, &readyfds))
		{
			printf("UDP server got a message\n");
			handle_udp(socket_udp, response);
		}
	}
	close(socket_tcp);
	close(socket_udp);
}

void handle_tcp(int socket_tcp, char *response)
{
	char buffer[BUFFER_SIZE];
	int client_fd, bytes_recv;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	client_fd = accept(socket_tcp, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd == -1)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	printf("TCP connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	bytes_recv = recv(client_fd, buffer, sizeof(buffer), 0);
	if (bytes_recv < 0)
	{
		perror("recv");
		close(client_fd);
		exit(EXIT_FAILURE);
	}
	printf("Receive: %s\n", buffer);
	if (send(client_fd, response, strlen(response), 0) == -1)
	{
		perror("send");
		close(client_fd);
		exit(EXIT_FAILURE);
	}
	close(client_fd);
}

void handle_udp(int socket_udp, char *response)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	char buffer[BUFFER_SIZE];
	int bytes_recv;

	bytes_recv = recvfrom(socket_udp, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
	if (bytes_recv < 0)
	{
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}
	buffer[bytes_recv] = '\0';

	printf("UDP connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	printf("Receive: %s\n", buffer);

	if (sendto(socket_udp, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_len) == -1)
	{
		perror("sendto");
		exit(EXIT_FAILURE);
	}
}

int init_udp()
{
	int sock;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT_UDP);

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("udp bind");
		close(sock);
		exit(EXIT_FAILURE);
	}

	return sock;
}

int init_tcp()
{
	int sock;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT_TCP);

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("tcp bind");
		close(sock);
		exit(EXIT_FAILURE);
	}

	if (listen(sock, 1) == -1)
	{
		perror("listen");
		close(sock);
		exit(EXIT_FAILURE);
	}

	return sock;
}