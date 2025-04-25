#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/server_tcp"
#define BUFFER_SIZE 1024

void run_server()
{
	struct sockaddr_un server_addr, client_addr;
	socklen_t server_len = sizeof(server_addr), client_len = sizeof(client_addr);
	int server_fd, bytes_rec, client_fd;
	char buffer[BUFFER_SIZE];

	server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

	if (server_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sun_family = AF_LOCAL;
	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
	unlink(SOCKET_PATH);

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 1) == -1)
	{
		perror("listen");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

	if (client_fd == -1)
	{
		perror("accept");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	bytes_rec = recv(client_fd, buffer, sizeof(buffer), 0);

	if (bytes_rec == -1)
	{
		perror("recv");
		close(server_fd);
		close(client_fd);
		exit(EXIT_FAILURE);
	}

	printf("Server receive: %s\n", buffer);

	buffer[bytes_rec] = '\0';
	buffer[0] = '2';

	if (send(client_fd, buffer, bytes_rec, 0) == -1)
	{
		perror("send");
		close(server_fd);
		close(client_fd);
		exit(EXIT_FAILURE);
	}

	close(server_fd);
	close(client_fd);
}

int main()
{
	run_server();
	return 0;
}