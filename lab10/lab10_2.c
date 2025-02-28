#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void printPIDandPPID(const char *process_name)
{
    printf("%s: PID = %d, PPID = %d\n", process_name, getpid(), getppid());
}

void createChileProcess(int id)
{
    pid_t pid = fork();
    char buffer[1024];

    if (pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        snprintf(buffer, 1024, "Child %d", id);
        printPIDandPPID(buffer);

        if (id == 1)
        {
            createChileProcess(3);
            createChileProcess(4);
        }
        else if (id == 2)
        {
            createChileProcess(5);
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        wait(NULL);
    }
}

int main()
{
    printPIDandPPID("Main");
    createChileProcess(1);
    createChileProcess(2);

    return 0;
}