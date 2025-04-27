#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7777
#define MULTICAST_IP "239.0.0.1"
#define BUFFER_SIZE 1024

void run_multicast_client()
{
	int server_fd;
	struct sockaddr_in addr;
	struct ip_mreq mreq;

	char buffer[BUFFER_SIZE];
	socklen_t addr_len = sizeof(addr);

	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	int yes = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		perror("setsockopt(SO_REUSEADDR)");
		exit(EXIT_FAILURE);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(server_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		perror("setsockopt");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		int recv_len = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &addr_len);
		if (recv_len < 0)
		{
			perror("recvfrom");
			close(server_fd);
			exit(EXIT_FAILURE);
		}
		buffer[recv_len] = '\0';
		printf("Received from server: %s\n", buffer);
	}

	close(server_fd);
}

int main()
{
	run_multicast_client();
	return 0;
}