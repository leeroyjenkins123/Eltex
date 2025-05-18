#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#define CLIENT_PORT 9999
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

void fill_ip_header(struct iphdr *ip_header, int message)
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
	ip_header->daddr = htonl(INADDR_LOOPBACK);
}

void fill_udp_header(struct udphdr *udp_header, int message)
{
	udp_header->uh_sport = htons(CLIENT_PORT);
	udp_header->uh_dport = htons(SERVER_PORT);
	udp_header->len = htons(sizeof(struct udphdr) + message);
	udp_header->check = 0;
}

void run_raw_client()
{
	int client;
	struct sockaddr_in server_addr;
	struct udphdr udp_header;
	struct iphdr ip_header;
	char buffer[BUFFER_SIZE], *message = "Hello?";

	client = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

	if (client == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	int one = 1;
	if (setsockopt(client, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0)
	{
		perror("setsockopt");
		close(client);
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	fill_udp_header(&udp_header, strlen(message));
	fill_ip_header(&ip_header, strlen(message));

	memcpy(buffer, &ip_header, sizeof(ip_header));
	memcpy(buffer + sizeof(ip_header), &udp_header, sizeof(udp_header));
	memcpy(buffer + sizeof(ip_header) + sizeof(udp_header), message, strlen(message));

	if (sendto(client, buffer, strlen(message) + sizeof(ip_header) + sizeof(udp_header), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
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

		struct iphdr *ip_hdr = (struct iphdr *)buffer;
		if (ip_hdr->protocol == IPPROTO_UDP)
		{
			struct udphdr *udp_hdr = (struct udphdr *)(buffer + (ip_hdr->ihl * 4));
			char *payload = buffer + (ip_hdr->ihl * 4) + sizeof(struct udphdr);
			int payload_len = ntohs(udp_hdr->len) - sizeof(struct udphdr);
			if (ntohs(udp_hdr->uh_dport) == CLIENT_PORT)
			{
				printf("Received from server: %.*s\n", payload_len, payload);
				break;
			}
		}
	}
	close(client);
}

int main()
{
	run_raw_client();
	return 0;
}