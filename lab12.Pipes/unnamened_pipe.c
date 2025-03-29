#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void process_handler(int fd[])
{
	pid_t pid;
	char buffer[20];

	pid = fork();

	if (pid < 0)
	{
		perror("Fork failed\n");
		exit(EXIT_FAILURE);
	}
	else if(pid == 0)
	{
		close(fd[1]);
		read(fd[0], &buffer, sizeof(buffer));
		printf("Child: %s\n", buffer);
		close(fd[0]);
		exit(EXIT_SUCCESS);
	}
	else
	{
		close(fd[0]);
		write(fd[1], "Hi!", 4);
		close(fd[1]);
		wait(NULL);
	}
	
}

int main()
{
	int fd[2];

	if (pipe(fd) < 0)
	{
		perror("Pipe failed\n");
		exit(EXIT_FAILURE);
	}

	process_handler(fd);

	return 0;
}