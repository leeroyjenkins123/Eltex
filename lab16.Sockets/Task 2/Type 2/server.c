#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#define MAIN_PORT 12345
#define BUFFER_SIZE 1024
#define SERVER_POOL_SIZE 5
#define NEW_PORT (MAIN_PORT + rand() % 10000 + 1)

struct socket_info
{
	int server_fd;
	int port;
	int is_busy;
	pthread_t thread_id;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
};

struct socket_info *server_pool;
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;

void sigint_handler(int sig);
void run_server();
void *server_thread(void *arg);

int main()
{
	run_server();
	return 0;
}

void sigint_handler(int sig)
{
	printf("Exiting...\n");
	for (int i = 0; i < SERVER_POOL_SIZE; i++)
	{
		pthread_cancel(server_pool[i].thread_id);
		close(server_pool[i].server_fd);
	}
	free(server_pool);
	exit(EXIT_SUCCESS);
}

void run_server()
{
	signal(SIGINT, sigint_handler);

	server_pool = (struct socket_info *)malloc(SERVER_POOL_SIZE * sizeof(struct socket_info));

	if (server_pool == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < SERVER_POOL_SIZE; i++)
	{
		server_pool[i].server_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (server_pool[i].server_fd == -1)
		{
			perror("socket");
			exit(EXIT_FAILURE);
		}

		server_pool[i].port = NEW_PORT;
		server_pool[i].is_busy = 0;
		server_pool[i].client_addr_len = sizeof(server_pool[i].client_addr);

		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addr.sin_port = htons(server_pool[i].port);

		if (bind(server_pool[i].server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		{
			perror("bind");
			close(server_pool[i].server_fd);
			exit(EXIT_FAILURE);
		}

		if (pthread_create(&server_pool[i].thread_id, NULL, server_thread, &server_pool[i]) != 0)
		{
			perror("pthread_create");
			close(server_pool[i].server_fd);
			exit(EXIT_FAILURE);
		}

		printf("Created server on port: %d\n", server_pool[i].port);
	}

	printf("Main server on port: %d\n", MAIN_PORT);

	int server_fd, bytes_recv;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);

	char buffer[BUFFER_SIZE], to_send[BUFFER_SIZE];

	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd == -1)
	{
		perror("main socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(MAIN_PORT);

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("main bind");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		bytes_recv = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
		if (bytes_recv == -1)
		{
			perror("recvfrom");
			close(server_fd);
			exit(EXIT_FAILURE);
		}

		int free_server = 0;

		pthread_mutex_lock(&pool_mutex);
		for (int i = 0; i < SERVER_POOL_SIZE; i++)
		{
			if (!server_pool[i].is_busy)
			{
				server_pool[i].is_busy = 1;
				sprintf(to_send, "%d", server_pool[i].port);
				if (sendto(server_fd, to_send, strlen(to_send)+1, 0, (struct sockaddr *)&client_addr, client_len) == -1)
				{
					perror("sendto");
					close(server_fd);
					exit(EXIT_FAILURE);
				}

				free_server = 1;
				break;
			}
		}
		pthread_mutex_unlock(&pool_mutex);

		if (!free_server)
		{
			char *message = "All server are busy. Please try later....";
			if (sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr, client_len) == -1)
			{
				perror("sendto");
				close(server_fd);
				exit(EXIT_FAILURE);
			}
		}
	}
	close(server_fd);
}

void *server_thread(void *arg)
{
	struct socket_info *info = (struct socket_info *)arg;
	char buffer[BUFFER_SIZE], response[BUFFER_SIZE];
	int bytes_read;
	time_t mytime = time(NULL);
	struct tm *now = localtime(&mytime);

	sprintf(response, "Date: %d.%d.%d\tTime: %d:%d:%d", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec);

	printf("Server listening from port %d\n", info->port);

	while (1)
	{
		bytes_read = recvfrom(info->server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&info->client_addr, &info->client_addr_len);
		if (bytes_read == -1)
		{
			perror("recvfrom");
			close(info->server_fd);
			exit(EXIT_FAILURE);
		}

		pthread_mutex_lock(&pool_mutex);
		info->is_busy = 1;
		pthread_mutex_unlock(&pool_mutex);

		printf("Server on port %d receive: %s\n", info->port, buffer);

		if (sendto(info->server_fd, response, strlen(response), 0,
				   (struct sockaddr *)&info->client_addr, info->client_addr_len) < 0)
		{
			perror("sendto");
			close(info->server_fd);
			exit(EXIT_FAILURE);
		}

		pthread_mutex_lock(&pool_mutex);
		info->is_busy = 0;
		pthread_mutex_unlock(&pool_mutex);
	}

	close(info->server_fd);
	pthread_exit(NULL);
}
