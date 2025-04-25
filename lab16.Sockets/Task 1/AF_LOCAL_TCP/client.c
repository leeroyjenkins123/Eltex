#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/server_tcp"
#define BUFFER_SIZE 1024

void run_client()
{
	struct sockaddr_un server_addr;
	int sock_fd, bytes_receive;
	char buffer[BUFFER_SIZE];
	char *message = "Hello!";

	sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sock_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sun_family = AF_LOCAL;
	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

	if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("connect");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	if (send(sock_fd, message, strlen(message) + 1, 0) == -1)
	{
		perror("send");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	bytes_receive = recv(sock_fd, buffer, sizeof(buffer), 0);
	if (bytes_receive == 0)
	{
		perror("recv");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	buffer[bytes_receive] = '\0';

	printf("Client receive: %s\n", buffer);

	close(sock_fd);
}

int main()
{
	run_client();
	return 0;
}