#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int get_signal_number(const char *sig_name)
{
	if (strcmp(sig_name, "SIGUSR1") == 0)
		return SIGUSR1;
	if (strcmp(sig_name, "SIGINT") == 0)
		return SIGINT;
	if (strcmp(sig_name, "SIGTERM") == 0)
		return SIGTERM;
	if (strcmp(sig_name, "SIGKILL") == 0)
		return SIGKILL;
	if (strcmp(sig_name, "SIGHUP") == 0)
		return SIGHUP;

	return -1;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <SIGNAL_NAME> <PID>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	const char *sig_name = argv[1];
	int signal = get_signal_number(sig_name);
	if (signal == -1)
	{
		fprintf(stderr, "Unknown signal: %s\n", sig_name);
		exit(EXIT_FAILURE);
	}

	pid_t pid = atoi(argv[2]);
	if (pid <= 0)
	{
		fprintf(stderr, "Invalid PID: %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	if (kill(pid, signal) == -1)
	{
		perror("kill");
		exit(EXIT_FAILURE);
	}

	printf("Sent %s to process %d\n", sig_name, pid);
	return 0;
}
