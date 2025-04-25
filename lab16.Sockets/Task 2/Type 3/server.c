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
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAIN_PORT 12345
#define BUFFER_SIZE 1024
#define SERVER_POOL_SIZE 5
#define MESSAGE_QUEUE_KEY 7777
#define NEW_PORT (MAIN_PORT + rand() % 10000 + 1)

struct socket_info
{
	int port;
	pthread_t thread_id;
};

struct message
{
	long msg_type;
	int client_fd;
};

struct socket_info *server_pool;
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;

void sigint_handler(int sig);
void run_server();
void *server_thread(void *arg);
void listener_socket_loop(int main_socket_fd, int msg_id);
void put_client_in_queue(int client_fd, int msg_id);
void handle_client(struct socket_info *info, int client_fd, char *buffer, char *response);

int main()
{
	run_server();

	return 0;
}

void handle_client(struct socket_info *info, int client_fd, char *buffer, char *response)
{
	int bytes_recv = recv(client_fd, buffer, sizeof(buffer), 0);
	time_t mytime = time(NULL);
	struct tm *now = localtime(&mytime);

	if (bytes_recv <= 0)
	{
		perror("recv");
		exit(EXIT_FAILURE);
	}
	else
	{
		sprintf(response, "Date: %d.%d.%d\tTime: %d:%d:%d", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec);
		printf("Receive from client from port %d - %s\n", info->port, buffer);
		if (send(client_fd, response, strlen(response), 0) == -1)
		{
			perror("send");
			exit(EXIT_FAILURE);
		}
		printf("Send to client on port: %d\n", info->port);
	}
	close(client_fd);
}

void put_client_in_queue(int client_fd, int msg_id)
{
	struct message msg;
	msg.client_fd = client_fd;
	msg.msg_type = 1;

	if (msgsnd(msg_id, &msg, sizeof(msg.client_fd), 0) == -1)
	{
		perror("msgsnd");
	}
}

void listener_socket_loop(int main_socket_fd, int msg_id)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	while (1)
	{
		int new_client = accept(main_socket_fd, (struct sockaddr *)&client_addr, &client_len);
		if (new_client == -1)
		{
			perror("accept");
			continue;
		}
		put_client_in_queue(new_client, msg_id);
	}
}

void *server_thread(void *arg)
{
	struct socket_info *info = (struct socket_info *)arg;
	char buffer[BUFFER_SIZE], response[BUFFER_SIZE];
	int msgid = msgget(MESSAGE_QUEUE_KEY, 0666 | IPC_CREAT);

	printf("I'm server on port: %d\n", info->port);

	while (1)
	{
		struct message msg;
		if (msgrcv(msgid, &msg, sizeof(msg.client_fd), 0, 0) == -1)
		{
			perror("msgrcv");
			continue;
		}
		handle_client(info, msg.client_fd, buffer, response);
	}
	pthread_exit(NULL);
}

void sigint_handler(int sig)
{
	printf("Exiting...\n");
	for (int i = 0; i < SERVER_POOL_SIZE; i++)
	{
		pthread_cancel(server_pool[i].thread_id);
	}
	free(server_pool);
	exit(EXIT_SUCCESS);
}

void run_server()
{
	signal(SIGINT, sigint_handler);

	int listener_fd, msg_id;
	struct sockaddr_in listener_addr;

	listener_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listener_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	listener_addr.sin_family = AF_INET;
	listener_addr.sin_addr.s_addr = INADDR_ANY;
	listener_addr.sin_port = htons(MAIN_PORT);

	if (bind(listener_fd, (struct sockaddr *)&listener_addr, sizeof(listener_addr)) == -1)
	{
		perror("bind");
		close(listener_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(listener_fd, 5) == -1)
	{
		perror("listen");
		close(listener_fd);
		exit(EXIT_FAILURE);
	}

	printf("Main server on port: %d\n", MAIN_PORT);

	server_pool = (struct socket_info *)malloc(SERVER_POOL_SIZE * sizeof(struct socket_info));
	if (server_pool == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < SERVER_POOL_SIZE; i++)
	{
		server_pool[i].port = NEW_PORT;

		if (pthread_create(&server_pool[i].thread_id, NULL, server_thread, &server_pool[i]) != 0)
		{
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
		printf("New server on port: %d\n", server_pool[i].port);
	}

	msg_id = msgget(MESSAGE_QUEUE_KEY, 0666 | IPC_CREAT);

	listener_socket_loop(listener_fd, msg_id);

	close(listener_fd);
}