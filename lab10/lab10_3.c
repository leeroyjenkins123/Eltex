#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define BUFFER_SIZE 1024
char g_directory[BUFFER_SIZE];

void updateDirectory()
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

void printHelloText()
{
    printf("\t\033[34mWelcome to the minimalistic equivalent of the bash command interpreter.\033[0m\n ");
    printf("\t\033[34mTo work with the bash command interpreter, write commands as in classic bash.\033[0m\n");
    printf("\t\033[34mTo exit the bash command interpreter, write exit.\033[0m\n");
    printf("\n");
}

void executeUserCommands(char *command)
{
    char *args[BUFFER_SIZE];
    int ind = 0;
    char *token = strtok(command, " \n");
    pid_t pid;

    while (token != NULL)
    {
        args[ind++] = token;
        token = strtok(NULL, " \n");
    }

    args[ind] = NULL;

    if (strncmp(args[0], "cd", sizeof("cd")) == 0)
    {
        if (args[1] != NULL)
        {
            if (chdir(args[1]) != 0)
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

    pid = fork();

    if (pid == -1)
    {
        perror("\033[31mFork failed\033[0m");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("Execvp failed.");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        wait(NULL);
    }
}

void runCommandInterpreterBash()
{
    char user_input_commands[BUFFER_SIZE];

    while (1)
    {
        updateDirectory();
        if (fgets(user_input_commands, BUFFER_SIZE, stdin) == NULL)
        {
            perror("\033[31mFgets failed\033[0m");
            continue;
        }
        user_input_commands[strcspn(user_input_commands, "\n")] = 0;

        if (strncmp(user_input_commands, "exit", sizeof("exit")) == 0)
        {
            break;
        }

        executeUserCommands(user_input_commands);
    }
}

int main()
{
    printHelloText();
    runCommandInterpreterBash();
    return 0;
}