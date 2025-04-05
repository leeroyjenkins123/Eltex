#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define BUFFER_SIZE 50
#define PERMISSIONS 0666
#define KEY_ID 50

struct msgbuf
{
    long mtype;
    char mtext[BUFFER_SIZE];
};

int main()
{
    key_t key;
    int msgid;
    struct msgbuf message;

    int fd = open("sysv_queue", O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    close(fd);

    key = ftok("sysv_queue", KEY_ID);

    if (key < 0)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    msgid = msgget(key, IPC_CREAT | PERMISSIONS);

    if (msgid < 0)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    message.mtype = 1;
    strcpy(message.mtext, "Hi!");
    if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    if (msgrcv(msgid, &message, sizeof(message.mtext), 2, 0) == -1)
    {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Received from client: %s\n", message.mtext);

    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}