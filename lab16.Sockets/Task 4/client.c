#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8888
#define BUFFER_SIZE 1024

void run_broadcast_client()
{

    int server_fd;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(addr);

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
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
    run_broadcast_client();
    return 0;
}