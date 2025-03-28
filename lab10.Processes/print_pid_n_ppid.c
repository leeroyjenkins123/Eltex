#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    pid = fork();

    if (pid < 0)
    {
        perror("Creation of process failed");
        return 1;
    }

    else if (pid == 0)
    {
        printf("Child: PID = %d\tPPID = %d\n", getpid(), getppid());
        exit(5);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);

        printf("Main: PID = %d\tPPID = %d\n", getpid(), getppid());
        printf("Child process ends with code: %d\n", WEXITSTATUS(status));
    }

    return 0;
}