#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define BUFFER_SIZE 1024
char g_directory[BUFFER_SIZE];

void update_directory()
{
    if (getcwd(g_directory, BUFFER_SIZE) == NULL)
    {
        strcpy(g_directory, ".");
    }
    char *home = getenv("HOME");
    if (home != NULL && strncmp(g_directory, home, strlen(home)) == 0)
    {
        printf("\033[32m~%s\033[0m$ ", g_directory + strlen(home));
    }
    else
    {
        printf("\033[32m%s\033[0m$ ", g_directory);
    }
}

void printf_hello_text()
{
    printf("\t\033[34mWelcome to the minimalistic equivalent of the bash command interpreter.\033[0m\n ");
    printf("\t\033[34mTo work with the bash command interpreter, write commands as in classic bash.\033[0m\n");
    printf("\t\033[34mTo exit the bash command interpreter, write exit.\033[0m\n");
    printf("\n");
}

void parse_command(char *command, char **leftArgs, char **rightArgs, int *isPipe)
{
    char *token = strtok(command, " \n");
    int argInd = 0;
    *isPipe = 0;

    while (token != NULL)
    {
        if (strcmp(token, "|") == 0)
        {
            *isPipe = 1;
            leftArgs[argInd] = NULL;
            argInd = 0;
            token = strtok(NULL, " \n");
            while (token != NULL)
            {
                rightArgs[argInd++] = token;
                token = strtok(NULL, " \n");
            }
            rightArgs[argInd] = NULL;
            return;
        }
        leftArgs[argInd++] = token;
        token = strtok(NULL, " \n");
    }
    leftArgs[argInd] = NULL;
}

void execute_user_commands(char *command)
{
    char *leftArgs[BUFFER_SIZE];
    char *rightArgs[BUFFER_SIZE];
    int isPipe;

    parse_command(command, leftArgs, rightArgs, &isPipe);

    if (strncmp(leftArgs[0], "cd", sizeof("cd")) == 0)
    {
        if (leftArgs[1] != NULL)
        {
            if (chdir(leftArgs[1]) != 0)
            {
                perror("\033[31mChdir failed\033[0m");
            }
        }
        else
        {
            fprintf(stderr, "\033[31mcd: missing argument.\033[0m\n");
        }
        return;
    }

    pid_t pid;
    int fd[2];

    if (isPipe)
    {
        if (pipe(fd) < 0)
        {
            perror("\033[31mpipe failed\033[37m");
            exit(EXIT_FAILURE);
        }
    }

    pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        if (isPipe)
        {
            pid_t pid2 = fork();

            if (pid2 == -1)
            {
                perror("\033[31mfork failed\033[37m");
                exit(EXIT_FAILURE);
            }
            else if (pid2 == 0)
            {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                if (execvp(leftArgs[0], leftArgs) == -1)
                {
                    perror("\033[31mExecvp failed\033[0m");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                if (execvp(rightArgs[0], rightArgs) == -1)
                {
                    perror("\033[31mExecvp failed\033[0m");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else
        {
            if (execvp(leftArgs[0], leftArgs) == -1)
            {
                perror("\033[31mExecvp failed\033[0m");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        if (isPipe)
        {
            close(fd[0]);
            close(fd[1]);
        }
        wait(NULL);
    }
}

void run_command_interpreter_bash()
{
    char userInputCommands[BUFFER_SIZE];

    while (1)
    {
        update_directory();
        if (fgets(userInputCommands, BUFFER_SIZE, stdin) == NULL)
        {
            perror("\033[31mFgets failed\033[0m");
            continue;
        }
        userInputCommands[strcspn(userInputCommands, "\n")] = 0;

        if (strncmp(userInputCommands, "exit", sizeof("exit")) == 0)
        {
            break;
        }

        execute_user_commands(userInputCommands);
    }
}

int main()
{
    printf_hello_text();
    run_command_interpreter_bash();
    return 0;
}