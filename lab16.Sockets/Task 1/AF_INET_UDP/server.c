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

void run_server()
{
	int sock_fd, bytes_rec;
	struct sockaddr_in server_addr, client_addr;
	socklen_t sockaddr_len = sizeof(struct sockaddr_in);
	char buffer[BUFFER_SIZE];

	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	if (bind(sock_fd, (struct sockaddr *)&server_addr, sockaddr_len) == -1)
	{
		perror("bind");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	bytes_rec = recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &sockaddr_len);

	if (bytes_rec == -1)
	{
		perror("bytes_rec");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	buffer[bytes_rec] = '\0';

	printf("Server receive from client: %s:%d - %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

	buffer[0] = '!';

	if (sendto(sock_fd, buffer, bytes_rec, 0, (struct sockaddr *)&client_addr, sockaddr_len) == -1)
	{
		perror("sendto");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	close(sock_fd);
}

int main()
{
	run_server();
	return 0;
}
