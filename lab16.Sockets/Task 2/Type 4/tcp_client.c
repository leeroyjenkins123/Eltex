#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 7777
#define BUFFER_SIZE 1024

void run_client(){
	int socket_fd;
	struct sockaddr_in server_addr;
	char buffer[BUFFER_SIZE];
	char *message = "Hello! Can I have date & time?";

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == -1)
	{
		perror("inet_pton");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}

	if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("connect");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}

	if (send(socket_fd, message, strlen(message), 0) == -1)
	{
		perror("send");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
	printf("Sent to server on port %d: %s\n", PORT, message);

	int bytes_read = read(socket_fd, buffer, BUFFER_SIZE);
	if (bytes_read < 0)
	{
		perror("read");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
	buffer[bytes_read] = '\0';

	printf("Received from server: %s\n", buffer);

	close(socket_fd);
}

int main(){
	run_client();
	return 0;
}