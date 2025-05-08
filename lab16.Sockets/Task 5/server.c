#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define IP_HEADER_LENGTH 20

void run_raw_server(){
	int socket_fd;
	char buffer[BUFFER_SIZE];

	socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

	if(socket_fd == -1){
		perror("socket");
		exit(EXIT_FAILURE);
	}

	while(1){
		int bytes_read = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);

		if(bytes_read < 0){
			perror("recvfrom");
			close(socket_fd);
			exit(EXIT_FAILURE);
		}
		
		
        int data_length = bytes_read - IP_HEADER_LENGTH;
        buffer[bytes_read] = '\0';  
		
        printf("UDP data (%d bytes):\n", data_length);
        for(int i = IP_HEADER_LENGTH; i < bytes_read; i++) {
            if(buffer[i] >= 32 && buffer[i] <= 126) {
                putchar(buffer[i]);
            } else {
                putchar('.');
            }
        }
        printf("\n\n");

	}
	close(socket_fd);
}

int main(){
	run_raw_server();
	return 0;
}