#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <signal.h>

#define SERVER_PORT 9090
#define BUFFER_SIZE 1024

uint16_t checksum(uint16_t *buffer, int len)
{
	unsigned long csum = 0;

	while (len > 1)
	{
		csum += *buffer++;
		len -= 2;
	}

	if (len == 1)
	{
		csum += *(uint8_t *)buffer;
	}

	csum = (csum >> 16) + (csum & 0xFFFF);
	csum += (csum >> 16);

	return ~csum;
}

void fill_ip_header(struct iphdr *ip_header, int message, char *server_ip)
{
	ip_header->version = 4;
	ip_header->ihl = 5;
	ip_header->tos = 0;
	ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + message);
	ip_header->id = 0;
	ip_header->frag_off = 0;
	ip_header->ttl = 255;
	ip_header->protocol = IPPROTO_UDP;
	ip_header->check = 0;
	ip_header->saddr = htonl(INADDR_LOOPBACK);
	ip_header->daddr = inet_addr(server_ip);
	ip_header->check = checksum((uint16_t *)ip_header, ip_header->ihl * 4);
}

void fill_udp_header(struct udphdr *udp_header, int message, uint16_t client_port)
{
	udp_header->uh_sport = htons(client_port);
	udp_header->uh_dport = htons(SERVER_PORT);
	udp_header->len = htons(sizeof(struct udphdr) + message);
	udp_header->check = 0;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Usage: %s <server_ip> <client_port>\n", argv[0]);
		return 1;
	}

	char *server_ip = argv[1];
	uint16_t client_port = atoi(argv[2]);

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (sockfd < 0)
	{
		perror("socket");
		return 1;
	}

	int optval = 1;
	if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0)
	{
		perror("setsockopt");
		close(sockfd);
		return 1;
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(SERVER_PORT),
		.sin_addr.s_addr = inet_addr(server_ip)};

	char buffer[BUFFER_SIZE];
	char send_buf[BUFFER_SIZE];
	char recv_buf[BUFFER_SIZE];
	struct iphdr ip_header;
	struct udphdr udp_header;
	int bytes_read = 0;

	signal(SIGINT, SIG_IGN);

	printf("Client started on port %d. Type messages or 'CLOSE' to exit or 'REMOVE' to remove client from server memory\n", client_port);

	while (1)
	{
		printf("> ");
		fflush(stdout);

		if (!fgets(buffer, BUFFER_SIZE, stdin))
		{
			perror("fgets");
			break;
		}

		size_t len = strlen(buffer);
		if (len > 0 && buffer[len - 1] == '\n')
		{
			buffer[len - 1] = '\0';
		}

		if (strlen(buffer) == 0)
			continue;

		if (strcmp(buffer, "CLOSE") == 0)
		{
			printf("Closing connection...\n");
			break;
		}

		fill_udp_header(&udp_header, strlen(buffer), client_port);
		fill_ip_header(&ip_header, strlen(buffer), server_ip);

		memcpy(send_buf, &ip_header, sizeof(ip_header));
		memcpy(send_buf + sizeof(ip_header), &udp_header, sizeof(udp_header));
		memcpy(send_buf + sizeof(ip_header) + sizeof(udp_header), buffer, strlen(buffer));

		if (sendto(sockfd, send_buf, sizeof(ip_header) + sizeof(udp_header) + strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		{
			perror("sendto");
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		printf("[Sent] %s\n", buffer);

		if (strcmp(buffer, "REMOVE") == 0)
		{
			printf("Removing from server and closing connection...\n");
			break;
		}

		while (1)
		{
			bytes_read = recvfrom(sockfd, recv_buf, BUFFER_SIZE, 0, NULL, NULL);
			if (bytes_read == -1)
			{
				perror("recv");
				break;
			}

			struct iphdr *recv_iph = (struct iphdr *)recv_buf;
			if (recv_iph->protocol != IPPROTO_UDP)
			{
				continue;
			}

			struct udphdr *recv_udph = (struct udphdr *)(recv_buf + (recv_iph->ihl * 4));
			if (ntohs(recv_udph->dest) != client_port)
			{
				continue;
			}

			char *payload = recv_buf + (recv_iph->ihl * 4) + sizeof(struct udphdr);
			int payload_len = ntohs(recv_udph->len) - sizeof(struct udphdr);
			payload[payload_len] = '\0';

			printf("[Received] %s\n", payload);
			break;
		}
	}

	close(sockfd);
	return 0;
}