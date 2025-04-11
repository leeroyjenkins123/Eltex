#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	sigset_t mask;

	int status, sig_num;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	status = sigprocmask(SIG_BLOCK, &mask, NULL);

	if (status == -1)
	{
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	printf("Waiting for SIGINT. PID: %d\n", getpid());

	while (1)
	{
		if (sigwait(&mask, &sig_num) != 0)
		{
			perror("sigwait");
			exit(EXIT_FAILURE);
		}

		printf("Got signal SIGINT%d\n", sig_num);
	}

	return 0;
}