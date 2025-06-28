#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define MAX_DRIVERS 10

#define CLI "\033[34m"
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"

enum DriverStatus
{
	AVAILABLE,
	BUSY
};

struct Driver
{
	pid_t pid;
	int cliToDriver[2];
	int driverToCli[2];
	enum DriverStatus status;
};

struct Driver drivers[MAX_DRIVERS];
pthread_mutex_t output = PTHREAD_MUTEX_INITIALIZER;
int driversCount = 0;

void print_menu();
void sigchld_handler(int sig);
void print_cli();
int handle_command();
void create_driver();
void send_task(pid_t pid, int time);
void get_status(pid_t pid);
void get_drivers();
struct Driver *find_driver(pid_t pid);
void handle_driver_response(fd_set *rfds);
void cleanup();
void sigint_handler(int sig);

int main()
{
	signal(SIGCHLD, sigchld_handler);
	signal(SIGINT, sigint_handler);
	print_menu();
	print_cli();

	while (1)
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO, &rfds);

		int maxFd = STDIN_FILENO;

		for (int i = 0; i < driversCount; i++)
		{
			FD_SET(drivers[i].driverToCli[0], &rfds);
			if (drivers[i].driverToCli[0] > maxFd)
			{
				maxFd = drivers[i].driverToCli[0];
			}
		}

		int ready = select(maxFd + 1, &rfds, NULL, NULL, NULL);

		if (FD_ISSET(STDIN_FILENO, &rfds))
		{
			if (handle_command() == 1)
			{
				break;
			}
		}
		else
		{
			handle_driver_response(&rfds);
		}
	}

	return 0;
}

void print_menu()
{
	printf("\t----------------------\n");
	printf("\tTaxi Managment Service\n");
	printf("\t----------------------\n");

	printf("\n\tCommands:\n");
	printf("1. create_driver\n");
	printf("2. send_task <pid> <task_time>\n");
	printf("3. get_status <pid>\n");
	printf("4. get_drivers\n");
	printf("5. exit\n");
}

void sigchld_handler(int sig)
{
	pthread_mutex_trylock(&output);
	printf("\nSIGCHLD handler triggered\n");
	(void)sig;
	pid_t pid;
	int status;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		for (int i = 0; i < driversCount; i++)
		{
			if (drivers[i].pid == pid)
			{
				close(drivers[i].cliToDriver[1]);
				close(drivers[i].driverToCli[0]);
				for (int j = i; j < driversCount - 1; j++)
				{
					drivers[j] = drivers[j + 1];
				}
				driversCount--;
				break;
			}
		}
	}
	pthread_mutex_unlock(&output);
}

void sigint_handler(int sig)
{
	(void)sig;
	cleanup();
	exit(0);
}

void print_cli()
{
	pthread_mutex_trylock(&output);
	printf(CLI "CLI> " RESET);
	fflush(stdout);
	pthread_mutex_unlock(&output);
}

int handle_command()
{
	char inputBuffer[BUFFER_SIZE];
	fgets(inputBuffer, BUFFER_SIZE, stdin);
	size_t length = strcspn(inputBuffer, "\n");
	inputBuffer[length] = '\0';

	if (strcmp(inputBuffer, "create_driver") == 0)
	{
		create_driver();
		return 0;
	}
	else if (strncmp(inputBuffer, "send_task ", 10) == 0)
	{
		pid_t pid = 0;
		int taskTime = 0;
		if (sscanf(inputBuffer + 10, "%d %d", &pid, &taskTime) == 2)
		{
			send_task(pid, taskTime);
		}
		else
		{
			fprintf(stderr, RED "Error! Usage: send_task <pid> <task_timer>\n" RESET);
			print_cli();
		}
		return 0;
	}
	else if (strncmp(inputBuffer, "get_status ", 11) == 0)
	{
		pid_t pid = 0;
		if (sscanf(inputBuffer + 11, "%d", &pid) == 1)
		{
			get_status(pid);
		}
		else
		{
			fprintf(stderr, RED "Error! Usage: get_status <pid>\n" RESET);
			print_cli();
		}
		return 0;
	}
	else if (strcmp(inputBuffer, "get_drivers") == 0)
	{
		if (driversCount == 0)
		{
			printf(RED "You have 0 drivers\n" RESET);
			print_cli();
		}
		else
		{
			get_drivers();
		}

		return 0;
	}
	else if (strcmp(inputBuffer, "exit") == 0)
	{
		cleanup();
		return 1;
	}
	else
	{
		fprintf(stderr, RED "Error! Unknown command\n" RESET);
		print_cli();
		return 0;
	}
	return 1;
}

struct Driver *find_driver(pid_t pid)
{
	for (int i = 0; i < driversCount; i++)
	{
		if (drivers[i].pid == pid)
		{
			return &drivers[i];
		}
	}
	return NULL;
}

void send_task(pid_t pid, int time)
{
	struct Driver *driver = find_driver(pid);
	if (!driver)
	{
		printf(RED "Driver %d not found\n" RESET, pid);
		print_cli();
		return;
	}

	char buffer[BUFFER_SIZE];
	int len = snprintf(buffer, BUFFER_SIZE, "TASK %d\n", time);
	write(driver->cliToDriver[1], buffer, len);
}

void get_status(pid_t pid)
{
	struct Driver *driver = find_driver(pid);
	if (!driver)
	{
		printf(RED "Driver %d not found\n" RESET, pid);
		print_cli();
		return;
	}
	const char *msg = "STATUS\n";
	write(drivers->cliToDriver[1], msg, strlen(msg));
}

void get_drivers()
{
	const char *msg = "STATUS\n";
	for (int i = 0; i < driversCount; i++)
	{
		write(drivers[i].cliToDriver[1], msg, strlen(msg));
	}
}

void create_driver()
{
	if (driversCount >= MAX_DRIVERS)
	{
		printf(RED "Drivers limit reached\n" RESET);
		return;
	}

	int cliToDriver[2], driverToCli[2];

	if (pipe(cliToDriver) == -1 || pipe(driverToCli) == -1)
	{
		perror("pipe");
		return;
	}
	pid_t pid = fork();

	if (pid == -1)
	{
		perror("fork");
		close(cliToDriver[0]);
		close(cliToDriver[1]);
		close(driverToCli[0]);
		close(driverToCli[1]);
		return;
	}

	if (pid == 0)
	{
		close(cliToDriver[1]);
		close(driverToCli[0]);

		char buffer[BUFFER_SIZE];
		enum DriverStatus status = AVAILABLE;
		time_t end_time = 0;

		while (1)
		{
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(cliToDriver[0], &rfds);

			struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
			int ready = select(cliToDriver[0] + 1, &rfds, NULL, NULL, &timeout);

			if (status == BUSY && time(NULL) >= end_time)
			{
				status = AVAILABLE;
				char msg[BUFFER_SIZE];
				int len = snprintf(msg, BUFFER_SIZE, GREEN "Available\n" RESET);
				write(driverToCli[1], msg, len);
			}

			if (ready > 0 && FD_ISSET(cliToDriver[0], &rfds))
			{
				int bytesRead = read(cliToDriver[0], buffer, BUFFER_SIZE);
				if (bytesRead <= 0)
					break;
				buffer[bytesRead] = '\0';

				if (strncmp(buffer, "TASK ", 5) == 0)
				{
					if (status == BUSY)
					{
						char msg[BUFFER_SIZE];
						int len = snprintf(msg, BUFFER_SIZE, RED "Busy %ld\n" RESET, end_time - time(NULL));
						write(driverToCli[1], msg, len);
					}
					else
					{
						int taskTime = atoi(buffer + 5);
						status = BUSY;
						end_time = time(NULL) + taskTime;
						char msg[BUFFER_SIZE];
						int len = snprintf(msg, BUFFER_SIZE, "Task started (%d sec)\n", taskTime);
						write(driverToCli[1], msg, len);
					}
				}
				else if (strncmp(buffer, "STATUS", 6) == 0)
				{
					char msg[BUFFER_SIZE];
					int len;
					if (status == BUSY)
					{
						len = snprintf(msg, BUFFER_SIZE, RED "Busy %ld\n" RESET, end_time - time(NULL));
					}
					else
					{
						len = snprintf(msg, BUFFER_SIZE, GREEN "Available\n" RESET);
					}
					write(driverToCli[1], msg, len);
				}
			}
		}

		close(cliToDriver[0]);
		close(driverToCli[1]);
		exit(0);
	}
	else
	{
		close(cliToDriver[0]);
		close(driverToCli[1]);

		drivers[driversCount].pid = pid;
		drivers[driversCount].status = AVAILABLE;
		drivers[driversCount].cliToDriver[1] = cliToDriver[1];
		drivers[driversCount].driverToCli[0] = driverToCli[0];
		driversCount++;

		printf("Created driver with PID: %d\n", pid);
		print_cli();
	}
}

void handle_driver_response(fd_set *rfds)
{
	char buffer[BUFFER_SIZE];
	pthread_mutex_trylock(&output);

	for (int i = 0; i < driversCount; i++)
	{
		if (FD_ISSET(drivers[i].driverToCli[0], rfds))
		{
			int bytesRead = read(drivers[i].driverToCli[0], buffer, BUFFER_SIZE);
			if (bytesRead > 0)
			{
				buffer[bytesRead] = '\0';
				printf("\nDriver %d: %s", drivers[i].pid, buffer);
			}
		}
	}
	pthread_mutex_unlock(&output);

	print_cli();
}

void cleanup()
{
	for (int i = 0; i < driversCount; i++)
	{
		kill(drivers[i].pid, SIGTERM);
	}

	for (int i = 0; i < driversCount; i++)
	{
		waitpid(drivers[i].pid, NULL, 0);
		close(drivers[i].driverToCli[0]);
		close(drivers[i].cliToDriver[1]);
	}
}
