#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	sigset_t mask;
	int sig;

	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);

	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
	{
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	printf("Waiting for SIGUSR1. PID: %d\n", getpid());

	while (1)
	{
		if (sigwait(&mask, &sig) != 0)
		{
			perror("sigwait");
			exit(EXIT_FAILURE);
		}

		if (sig == SIGUSR1)
		{
			printf("Received SIGUSR1 in sigwait\n");
		}
	}

	return 0;
}