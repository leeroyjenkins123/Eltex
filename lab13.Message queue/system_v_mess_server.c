#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>

#define REGISTER 1
#define MESSAGE 2
#define UNREGISTER 3
#define NEW_USER 4
#define CHAT_MSG 5
#define USER_LEFT 6

typedef struct
{
    char name[32];
    int qid;
} Client;

typedef struct history_entry
{
    char sender[32];
    char text[256];
    struct history_entry *next;
} HistoryEntry;

struct msg_buffer
{
    long mtype;
    char data[288];
};

Client clients[100];
int client_count = 0;
HistoryEntry *history_head = NULL;
int server_qid;

void add_to_history(const char *sender, const char *text)
{
    HistoryEntry *entry = malloc(sizeof(HistoryEntry));
    strncpy(entry->sender, sender, 32);
    strncpy(entry->text, text, 256);
    entry->next = history_head;
    history_head = entry;
}

void send_history(int client_qid)
{
    HistoryEntry *current = history_head;
    HistoryEntry *prev = NULL;
    HistoryEntry *next;

    while (current != NULL)
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    current = prev;
    while (current != NULL)
    {
        struct msg_buffer msg;
        msg.mtype = CHAT_MSG;
        memcpy(msg.data, current->sender, 32);
        memcpy(msg.data + 32, current->text, 256);
        msgsnd(client_qid, &msg, sizeof(msg.data), 0);
        current = current->next;
    }
}

void notify_new_user(const char *name)
{
    struct msg_buffer msg;
    msg.mtype = NEW_USER;
    memcpy(msg.data, name, 32);
    for (int i = 0; i < client_count; i++)
    {
        msgsnd(clients[i].qid, &msg, sizeof(msg.data), 0);
    }
}

void broadcast_message(const char *sender, const char *text)
{
    struct msg_buffer msg;
    msg.mtype = CHAT_MSG;
    memcpy(msg.data, sender, 32);
    memcpy(msg.data + 32, text, 256);
    for (int i = 0; i < client_count; i++)
    {
        msgsnd(clients[i].qid, &msg, sizeof(msg.data), 0);
    }
}

void cleanup(int sig)
{
    msgctl(server_qid, IPC_RMID, NULL);
    exit(0);
}

int main()
{
    signal(SIGINT, cleanup);
    key_t key = ftok("server.c", 1);
    server_qid = msgget(key, IPC_CREAT | 0666);

    struct msg_buffer msg;
    while (1)
    {
        if (msgrcv(server_qid, &msg, sizeof(msg.data), 0, 0) == -1)
        {
            perror("msgrcv");
            continue;
        }
        if (msg.mtype == REGISTER)
        {
            char name[32];
            int client_qid;
            memcpy(name, msg.data, 32);
            memcpy(&client_qid, msg.data + 32, sizeof(int));
            clients[client_count].qid = client_qid;
            strncpy(clients[client_count].name, name, 32);
            client_count++;
            send_history(client_qid);

            for (int i = 0; i < client_count - 1; i++)
            { 
                struct msg_buffer user_msg;
                user_msg.mtype = NEW_USER;
                memcpy(user_msg.data, clients[i].name, 32);
                msgsnd(client_qid, &user_msg, sizeof(user_msg.data), 0);
            }

            notify_new_user(name);
            printf("new connection: %d\n", client_qid);
        }
        else if (msg.mtype == MESSAGE)
        {
            char sender[32], text[256];
            memcpy(sender, msg.data, 32);
            memcpy(text, msg.data + 32, 256);
            add_to_history(sender, text);
            broadcast_message(sender, text);
        }
        else if (msg.mtype == UNREGISTER)
        {
            int client_qid;
            char name[32];
            memcpy(name, msg.data, 32);
            memcpy(&client_qid, msg.data + 32, sizeof(int));

            for (int i = 0; i < client_count; i++)
            {
                if (clients[i].qid == client_qid)
                {
                    struct msg_buffer leave_msg;
                    leave_msg.mtype = USER_LEFT;
                    memcpy(leave_msg.data, name, 32);
                    for (int j = 0; j < client_count; j++)
                    {
                        if (j != i)
                        {
                            msgsnd(clients[j].qid, &leave_msg, sizeof(leave_msg.data), 0);
                        }
                    }

                    memmove(&clients[i], &clients[i + 1], (client_count - i - 1) * sizeof(Client));
                    client_count--;
                    break;
                }
            }
        }
    }
    return 0;
}