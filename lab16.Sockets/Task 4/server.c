#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8888
#define BROADCAST_IP "255.255.255.255"
#define MESSAGE "Hello, from broadcast server!"
#define TIMEOUT 3


void run_broadcast_server(){

    int server_fd;
    struct sockaddr_in server_addr;
    int broadcast_permission = 1;


    if((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_permission, sizeof(broadcast_permission)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);
	printf("BROADCAST SERVER\t\n");

    while(1){
        if(sendto(server_fd, MESSAGE, strlen(MESSAGE), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
            perror("sendto");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        printf("Sent message: %s\n", MESSAGE);
        sleep(TIMEOUT);
    }
    close(server_fd);
}

int main() {
    run_broadcast_server();
    return 0;
}