#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void run_client()
{
	int sock_fd, bytes_rec;
	struct sockaddr_in server_addr, client_addr;
	socklen_t sockaddr_len = sizeof(struct sockaddr_in);
	char buffer[BUFFER_SIZE];
	char *message = "Hello!";

	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	client_addr.sin_port = 0;

	if (bind(sock_fd, (struct sockaddr *)&client_addr, sockaddr_len) == -1)
	{
		perror("bind");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	if (sendto(sock_fd, message, strlen(message) + 1, 0, (struct sockaddr *)&server_addr, sockaddr_len) == -1)
	{
		perror("sendto");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	bytes_rec = recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &sockaddr_len);

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
