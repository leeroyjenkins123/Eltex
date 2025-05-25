#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <ifaddrs.h>
#include <net/if.h>

#define SERVER_PORT 12345
#define CLIENT_PORT 9999
#define BUFFER_SIZE 1500
#define SERVER_IP "192.168.0.104"
#define CLIENT_IP "192.168.0.164"
#define SERVER_MAC {0x94, 0xe9, 0x79, 0xa9, 0xfc, 0xcb}
#define CLIENT_MAC {0x08, 0x00, 0x27, 0x15, 0xe9, 0xcd}
#define CLIENT_IF "enp0s3"

int checksum(uint16_t *buffer, int len)
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

void fill_ip_header(struct iphdr *ip_header, int message)
{
	ip_header->ihl = 5;
	ip_header->version = 4;
	ip_header->tos = 0;
	ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + message);
	ip_header->id = 2;
	ip_header->frag_off = 0;
	ip_header->ttl = 255;
	ip_header->protocol = IPPROTO_UDP;
	ip_header->saddr = inet_addr(CLIENT_IP);
	ip_header->daddr = inet_addr(SERVER_IP);
	ip_header->check = 0;
	ip_header->check = checksum((uint16_t *)ip_header, ip_header->ihl * 4);
}

void fill_ethernet_header(struct ethhdr *eth_header, unsigned char *source_mac, unsigned char *dest_mac)
{
	memcpy(eth_header->h_dest, dest_mac, 6);
	memcpy(eth_header->h_source, source_mac, 6);
	eth_header->h_proto = htons(ETH_P_IP);
}

void fill_udp_header(struct udphdr *udp_header, int message)
{
	udp_header->uh_sport = htons(CLIENT_PORT);
	udp_header->uh_dport = htons(SERVER_PORT);
	udp_header->len = htons(sizeof(struct udphdr) + message);
	udp_header->check = 0;
}

void run_client()
{
	int fd;
	char buffer[BUFFER_SIZE];
	char *message = "Hi?";
	struct sockaddr_ll addr = {0};
	struct ethhdr eth_header;
	struct iphdr ip_header;
	struct udphdr udp_header;
	unsigned char source_mac[6] = CLIENT_MAC, dest_mac[6] = SERVER_MAC;

	fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	printf("Client started on interface %s\n", CLIENT_IF);

	fill_ethernet_header(&eth_header, source_mac, dest_mac);
	fill_udp_header(&udp_header, strlen(message));
	fill_ip_header(&ip_header, strlen(message));
	memcpy(buffer, &eth_header, sizeof(eth_header));
	memcpy(buffer + sizeof(eth_header), &ip_header, sizeof(ip_header));
	memcpy(buffer + sizeof(eth_header) + sizeof(ip_header), &udp_header, sizeof(udp_header));
	memcpy(buffer + sizeof(eth_header) + sizeof(ip_header) + sizeof(udp_header), message, strlen(message));

	addr.sll_ifindex = if_nametoindex(CLIENT_IF);
	addr.sll_halen = ETH_ALEN;
	memcpy(addr.sll_addr, dest_mac, 6);

	sendto(fd, buffer, sizeof(eth_header) + sizeof(ip_header) + sizeof(udp_header) + strlen(message), 0, (struct sockaddr *)&addr, sizeof(addr));

	while (1)
	{
		ssize_t bytes_received = recvfrom(fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
		if (bytes_received == -1)
		{
			perror("recvfrom");
			close(fd);
			exit(EXIT_FAILURE);
		}

		struct ethhdr *eth_hdr = (struct ethhdr *)buffer;

		if (ntohs(eth_hdr->h_proto) == ETH_P_IP)
		{
			struct iphdr *ip_hdr = (struct iphdr *)(buffer + sizeof(struct ethhdr));
			if (ip_hdr->protocol == IPPROTO_UDP)
			{
				struct udphdr *udp_hdr = (struct udphdr *)(buffer + sizeof(struct ethhdr) + (ip_hdr->ihl * 4));
				char *payload = buffer + sizeof(struct ethhdr) + (ip_hdr->ihl * 4) + sizeof(struct udphdr);
				int payload_len = ntohs(udp_hdr->len) - sizeof(struct udphdr);

				if (ntohs(udp_hdr->uh_dport) == CLIENT_PORT)
				{
					printf("Received from server on port: %d -  %.*s\n", SERVER_PORT, payload_len, payload);
					break;
				}
			}
		}
	}

	close(fd);
}

int main()
{
	run_client();
	return 0;
}