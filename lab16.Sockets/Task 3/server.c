#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 7777
#define MULTICAST_IP "239.0.0.1"
#define MESSAGE "Hello, from multicast server!"
#define TIMEOUT 4

void run_multicast_server()
{
	int server_fd;
	struct sockaddr_in server_addr;

	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(MULTICAST_IP);
	printf("MULTICAST SERVER\t\n");

	while (1)
	{
		if (sendto(server_fd, MESSAGE, strlen(MESSAGE), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		{
			perror("sendto");
			close(server_fd);
			exit(EXIT_FAILURE);
		}
		printf("Sent message: %s\n", MESSAGE);
		sleep(TIMEOUT);
	}

	close(server_fd);
}

int main()
{
	run_multicast_server();
	return 0;
}