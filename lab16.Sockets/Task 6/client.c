#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/udp.h>

#define CLIENT_PORT 9999
#define SERVER_PORT 12345
#define OFFSET 8
#define BUFFER_SIZE 1024

void fill_header(struct udphdr *header, int message)
{
	header->uh_sport = htons(CLIENT_PORT);
	header->uh_dport = htons(SERVER_PORT);
	header->len = htons(OFFSET + message);
	header->check = 0;
}

void run_raw_client()
{
	int client;
	struct sockaddr_in server_addr;
	struct udphdr header;
	char buffer[BUFFER_SIZE], *message = "Hello?";

	client = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

	if (client == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	;
	server_addr.sin_port = htons(SERVER_PORT);

	fill_header(&header, strlen(message));

	memcpy(buffer, &header, OFFSET);
	memcpy(buffer + OFFSET, message, strlen(message));

	if (sendto(client, buffer, strlen(message) + OFFSET, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("sendto");
		close(client);
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		int bytes_rec = recvfrom(client, buffer, BUFFER_SIZE, 0, NULL, NULL);
		if (bytes_rec < 0)
		{
			perror("recvfrom");
			close(client);
			exit(EXIT_FAILURE);
		}
		struct udphdr *recv_udp = (struct udphdr *)(buffer + 20); // Пропускаем IP-заголовок
		char *udp_data = (char *)(buffer + 20 + 8);				  // Пропускаем IP (20) + UDP (8) заголовки

		// Проверяем, что это ответ нашему клиенту
		if (ntohs(recv_udp->uh_dport) == CLIENT_PORT)
		{
			printf("Received from server: %s\n", udp_data);
			break;
		}
	}
	close(client);
}

int main()
{
	run_raw_client();
	return 0;
}