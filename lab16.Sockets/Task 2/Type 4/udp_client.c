#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 7778
#define BUFFER_SIZE 1024

void run_client(){
	int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(serv_addr);
	char *message = "Hello! Can I have date & time?";

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (sendto(sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, addr_len) == -1) {
        perror("sendto");
        close(sock);
        exit(EXIT_FAILURE);
    }

	printf("Sent to server on port %d: %s\n", PORT, message);

    int bytes_read = recvfrom(sock, buffer, BUFFER_SIZE, 0,
                                 (struct sockaddr *)&serv_addr, &addr_len);
    if (bytes_read == -1) {
        perror("recvfrom");
        close(sock);
        exit(EXIT_FAILURE);
    }

	buffer[bytes_read] = '\0';

	printf("Received from server: %s\n", buffer);
    close(sock);
}

int main(){
	run_client();
	return 0;
}