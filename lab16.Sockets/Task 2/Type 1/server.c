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
#define NEW_PORT (MAIN_PORT + rand() % 10000 + 1)
#define BUFFER_SIZE 1024
int server_fd = 0;

void run_server();
void sigint_handler(int sig);
void *create_new_server(void *arg);

int main()
{
	run_server();
	return 0;
}

void sigint_handler(int sig)
{
	printf("Exiting...\n");
	close(server_fd);
	exit(EXIT_SUCCESS);
}

void run_server()
{
	signal(SIGINT, sigint_handler);

	struct sockaddr_in server_addr, client_addr;
	int new_fd, new_port;
	socklen_t client_len = sizeof(server_addr);
	char buffer[BUFFER_SIZE];

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(MAIN_PORT);

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 5) == -1)
	{
		perror("listen");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("Main server on port: %d\n", MAIN_PORT);

	while (1)
	{
		new_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
		if (new_fd == -1)
		{
			perror("accept");
			close(server_fd);
			exit(EXIT_FAILURE);
		}

		new_port = NEW_PORT;

		int *port = malloc(sizeof(int));

		if (port == NULL)
		{
			perror("malloc");
			close(new_fd);
			close(server_fd);
			exit(EXIT_FAILURE);
		}

		*port = new_port;

		pthread_t thread;
		if (pthread_create(&thread, NULL, create_new_server, port) < 0)
		{
			perror("pthread_create");
			close(new_fd);
			close(server_fd);
			exit(EXIT_FAILURE);
		}

		sprintf(buffer,"%d", new_port);
		if (send(new_fd, buffer, sizeof(buffer), 0) == -1)
		{
			perror("send");
			close(new_fd);
			close(server_fd);
			exit(EXIT_FAILURE);
		}

		close(new_fd);
	}
	close(server_fd);
}

void *create_new_server(void *arg)
{
	int new_port = *((int *)arg);
	free(arg);

	int new_server_fd, client_fd, bytes_recv;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);
	char buffer[BUFFER_SIZE], response[BUFFER_SIZE];
	time_t mytime = time(NULL);
	struct tm *now = localtime(&mytime);

	snprintf(response, BUFFER_SIZE, "Date: %d.%d.%d\tTime: %d:%d:%d", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec);

	new_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (new_server_fd == -1)
	{
		perror("new socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(new_port);

	if (bind(new_server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("new bind");
		close(new_server_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(new_server_fd, 1) == -1)
	{
		perror("new listen");
		close(new_server_fd);
		exit(EXIT_FAILURE);
	}

	printf("New server listen port: %d\n", new_port);

	client_fd = accept(new_server_fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd == -1)
	{
		perror("new accept");
		close(new_server_fd);
		exit(EXIT_FAILURE);
	}

	bytes_recv = recv(client_fd, buffer, sizeof(buffer), 0);
	if (bytes_recv == 0)
	{
		perror("recv");
		close(client_fd);
		close(new_server_fd);
		exit(EXIT_FAILURE);
	}

	printf("New server receive: %s\n", buffer);

	if (send(client_fd, response, sizeof(response), 0) == -1)
	{
		perror("new send");
		close(client_fd);
		close(new_server_fd);
		exit(EXIT_FAILURE);
	}

	close(client_fd);
	close(new_server_fd);
	return NULL;
}