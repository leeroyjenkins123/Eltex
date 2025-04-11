#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CLIENTS 100
#define MAX_MESSAGES 1000
#define USERNAME_LEN 32
#define MESSAGE_LEN 256

typedef struct {
    char name[USERNAME_LEN];
    int active;
} Client;

typedef struct {
    char sender[USERNAME_LEN];
    char text[MESSAGE_LEN];
} ChatMessage;

typedef struct {
    sem_t mutex;
    Client clients[MAX_CLIENTS];
    int client_count;
    ChatMessage messages[MAX_MESSAGES];
    int message_count;
} SharedMemory;

SharedMemory *shm;
int shm_id;

void cleanup(int sig) {
    shmdt(shm);
    shmctl(shm_id, IPC_RMID, NULL);
    exit(0);
}

void broadcast_user_update() {
    sem_wait(&shm->mutex);
    for (int i = 0; i < shm->client_count; i++) {
        shm->clients[i].active = 1;
    }
    sem_post(&shm->mutex);
}

void add_message(const char *sender, const char *text) {
    sem_wait(&shm->mutex);
    if (shm->message_count < MAX_MESSAGES) {
        strncpy(shm->messages[shm->message_count].sender, sender, USERNAME_LEN);
        strncpy(shm->messages[shm->message_count].text, text, MESSAGE_LEN);
        shm->message_count++;
    } else {
        for (int i = 0; i < MAX_MESSAGES - 1; i++) {
            shm->messages[i] = shm->messages[i + 1];
        }
        strncpy(shm->messages[MAX_MESSAGES - 1].sender, sender, USERNAME_LEN);
        strncpy(shm->messages[MAX_MESSAGES - 1].text, text, MESSAGE_LEN);
    }
    sem_post(&shm->mutex);
}

int main() {
    signal(SIGINT, cleanup);
    
    key_t key = ftok("server.c", 1);
    shm_id = shmget(key, sizeof(SharedMemory), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }
    
    shm = (SharedMemory *)shmat(shm_id, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    
    sem_init(&shm->mutex, 1, 1);
    shm->client_count = 0;
    shm->message_count = 0;
    
    printf("Server started. Shared memory ID: %d\n", shm_id);
    
    while (1) {
        sleep(1);
    }
    
    return 0;
}