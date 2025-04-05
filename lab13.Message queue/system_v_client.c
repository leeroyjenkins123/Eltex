#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

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

    key = ftok("sysv_queue", KEY_ID);

    if (key < 0)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    msgid = msgget(key, PERMISSIONS);

    if (msgid < 0)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    if (msgrcv(msgid, &message, sizeof(message.mtext), 1, 0) == -1)
    {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Received from server: %s\n", message.mtext);

    message.mtype = 2;
    strcpy(message.mtext, "Hello!");
    if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    return 0;
}