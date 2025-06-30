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
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

struct client_state
{
	uint32_t ip;
	uint16_t port;
	int count;
};

struct client_state clients[MAX_CLIENTS];
int clients_count = 0;

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

struct client_state *find_client(uint32_t ip, uint16_t port)
{
	for (int i = 0; i < clients_count; i++)
	{
		if (clients[i].ip == ip && clients[i].port == port)
		{
			return &clients[i];
		}
	}
	return NULL;
}

void add_client(uint32_t ip, uint16_t port)
{
	if (clients_count >= MAX_CLIENTS)
		return;
	clients[clients_count].ip = ip;
	clients[clients_count].port = port;
	clients[clients_count].count = 0;
	clients_count++;
}

void remove_client(uint32_t ip, uint16_t port)
{
	for (int i = 0; i < clients_count; i++)
	{
		if (clients[i].ip == ip && clients[i].port == port)
		{
			clients[i] = clients[clients_count - 1];
			clients_count--;
			printf("Client %s:%d removed\n",
				   inet_ntoa(*(struct in_addr *)&ip), ntohs(port));
			return;
		}
	}
}

void fill_ip_header(struct iphdr *ip_header, int message, struct iphdr ip_receive)
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
	ip_header->saddr = ip_receive.daddr;
	ip_header->daddr = ip_receive.saddr;
	ip_header->check = checksum((uint16_t *)ip_header, ip_header->ihl * 4);
}

void fill_udp_header(struct udphdr *udp_header, int message, uint16_t client_port)
{
	udp_header->uh_sport = htons(SERVER_PORT);
	udp_header->uh_dport = client_port;
	udp_header->len = htons(sizeof(struct udphdr) + message);
	udp_header->check = 0;
}

int main()
{

	int server;
	struct sockaddr_in client_addr;
	struct udphdr udp_header;
	struct iphdr ip_header;
	char buffer[BUFFER_SIZE];
	int bytes_read = 0;
	socklen_t client_len = sizeof(client_addr);

	server = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (server == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	int optval = 1;
	if (setsockopt(server, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0)
	{
		perror("setsockopt");
		close(server);
		exit(EXIT_FAILURE);
	}

	printf("---SERVER running on port %d---\n", SERVER_PORT);

	while (1)
	{
		bytes_read = recvfrom(server, buffer, BUFFER_SIZE, 0, NULL, NULL);
		if (bytes_read == -1)
		{
			perror("recvfrom");
			close(server);
			exit(EXIT_FAILURE);
		}

		struct iphdr *recv_iph = (struct iphdr *)buffer;
		if (recv_iph->protocol != IPPROTO_UDP)
		{
			continue;
		}

		struct udphdr *recv_udph = (struct udphdr *)(buffer + (recv_iph->ihl * 4));
		if (htons(recv_udph->uh_dport) != SERVER_PORT)
		{
			continue;
		}

		char *payload = buffer + (recv_iph->ihl * 4) + sizeof(struct udphdr);
		int payload_len = htons(recv_udph->len) - sizeof(struct udphdr);

		payload[payload_len] = '\0';

		uint32_t client_ip = recv_iph->saddr;
		uint16_t client_port = recv_udph->uh_sport;

		if (strncmp(payload, "REMOVE", 5) == 0)
		{
			remove_client(client_ip, client_port);
			continue;
		}

		struct client_state *client = find_client(client_ip, client_port);

		if (!client)
		{
			printf("New connection from %s:%d\n", inet_ntoa(*(struct in_addr *)&client_ip), ntohs(client_port));
			add_client(client_ip, client_port);
			client = &clients[clients_count - 1];
		}

		client->count++;

		char reply[BUFFER_SIZE];
		int reply_len = snprintf(reply, BUFFER_SIZE, "%s %d", payload, client->count);

		fill_udp_header(&udp_header, reply_len, client_port);
		fill_ip_header(&ip_header, reply_len, *recv_iph);

		char send_buffer[BUFFER_SIZE];

		memcpy(send_buffer, &ip_header, sizeof(ip_header));
		memcpy(send_buffer + sizeof(ip_header), &udp_header, sizeof(udp_header));
		memcpy(send_buffer + sizeof(ip_header) + sizeof(udp_header), reply, strlen(reply));

		client_addr.sin_family = AF_INET;
		client_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

		if (sendto(server, send_buffer, reply_len + sizeof(ip_header) + sizeof(udp_header), 0, (struct sockaddr *)&client_addr, client_len) == -1)
		{
			perror("sendto");
			close(server);
			exit(EXIT_FAILURE);
		}
	}

	close(server);
	return 0;
}