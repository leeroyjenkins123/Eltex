#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <bits/sigaction.h>

void sigusr1_handler(int signo){
	printf("Received SIGUSR1 signal (%d)\n", signo);
}

int main(){

	printf("PID of process: %d\n", getpid());

	struct sigaction sig_action;
    sig_action.sa_handler = &sigusr1_handler;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;

	if (sigaction(SIGUSR1, &sig_action, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

	while(1){
		sleep(1);
	}

	return 0;
}