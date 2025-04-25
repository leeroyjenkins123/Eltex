#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define SERVER_SOCKET_PATH "/tmp/server_udp"
#define BUFFER_SIZE 1024

void run_server()
{
	struct sockaddr_un server_addr, client_addr;

	char buffer[BUFFER_SIZE];

	socklen_t client_len = sizeof(client_addr), server_len = sizeof(server_addr);

	int sock_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);

	if (sock_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sun_family = AF_LOCAL;
	strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

	unlink(SERVER_SOCKET_PATH);

	if(bind(sock_fd, (struct sockaddr*)&server_addr, server_len) == -1){
		perror("bind");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	int bytes_rec = recvfrom(sock_fd, buffer, sizeof(buffer),0,(struct sockaddr*)&client_addr, &client_len);
	if(bytes_rec == -1){
		perror("recvfrom");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	buffer[bytes_rec] = '\0';

	printf("Server receive: %s\n", buffer); 

	buffer[0] = '9';

	if(sendto(sock_fd, buffer, bytes_rec, 0, (struct sockaddr*)&client_addr, client_len) == -1){
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