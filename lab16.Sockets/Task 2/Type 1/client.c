#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void run_client()
{
	int sock_fd, bytes_rec;
	struct sockaddr_in server_addr;
	socklen_t sockaddr_len = sizeof(struct sockaddr_in);
	char buffer[BUFFER_SIZE];
	char *message = "Hello! Can I have date & time?";

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
	{
		perror("inet_pton failed");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	if (connect(sock_fd, (struct sockaddr *)&server_addr, sockaddr_len) == -1)
	{
		perror("connect");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	bytes_rec = recv(sock_fd, buffer, BUFFER_SIZE, 0);
	if (bytes_rec < 0)
	{
		perror("read");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}
	int new_port = atoi(buffer);

	printf("New port: %d\n", new_port);
	close(sock_fd);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_port = htons(new_port);

	if (connect(sock_fd, (struct sockaddr *)&server_addr, sockaddr_len) == -1)
	{
		perror("connect");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	if (send(sock_fd, message, strlen(message) + 1, 0) == -1)
	{
		perror("sendto");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	bytes_rec = recv(sock_fd, buffer, sizeof(buffer), 0);

	if (bytes_rec == -1)
	{
		perror("bytes_rec");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	buffer[bytes_rec] = '\0';

	printf("Client receive from server: %s:%d - %s\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), buffer);

	close(sock_fd);
}

int main()
{
	run_client();
	return 0;
}