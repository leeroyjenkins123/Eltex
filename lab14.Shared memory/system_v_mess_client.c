#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <semaphore.h>

#define USERNAME_LEN 32
#define MESSAGE_LEN 256
#define MAX_CLIENTS 100
#define MAX_MESSAGES 1000

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
char username[USERNAME_LEN];
WINDOW *msg_win, *user_win, *input_win;
int running = 1;

void cleanup() {
    sem_wait(&shm->mutex);
    for (int i = 0; i < shm->client_count; i++) {
        if (strcmp(shm->clients[i].name, username) == 0) {
            shm->clients[i].active = 0;
            break;
        }
    }
    sem_post(&shm->mutex);
    shmdt(shm);
    endwin();
}

void sigint_handler(int sig) {
    running = 0;
}

void update_user_list() {
    werase(user_win);
    box(user_win, 0, 0);
    wattron(user_win, A_REVERSE);
    mvwprintw(user_win, 0, 2, "Users (%d)", shm->client_count);
    wattroff(user_win, A_REVERSE);
    
    int row = 1;
    for (int i = 0; i < shm->client_count; i++) {
        if (shm->clients[i].active) {
            if (strcmp(shm->clients[i].name, username) == 0) {
                wattron(user_win, A_BOLD);
                mvwprintw(user_win, row++, 2, "> %s (you)", shm->clients[i].name);
                wattroff(user_win, A_BOLD);
            } else {
                mvwprintw(user_win, row++, 2, "- %s", shm->clients[i].name);
            }
        }
    }
    wrefresh(user_win);
}

void *receiver_thread(void *arg) {
    int last_message_count = 0;
    int last_client_count = 0;
    int msg_row = 1;
    
    sem_wait(&shm->mutex);
    for (int i = 0; i < shm->message_count; i++) {
        if (msg_row >= 40) {
            wscrl(msg_win, 1);
            msg_row = 39;
        }
        mvwprintw(msg_win, msg_row++, 2, "%s: %s", 
                 shm->messages[i].sender, shm->messages[i].text);
    }
    last_message_count = shm->message_count;

    update_user_list();
    last_client_count = shm->client_count;
    sem_post(&shm->mutex);
    wrefresh(msg_win);

    while (running) {
        sem_wait(&shm->mutex);
        
        if (shm->message_count > last_message_count) {
            for (int i = last_message_count; i < shm->message_count; i++) {
                if (msg_row >= 40) {
                    wscrl(msg_win, 1);
                    msg_row = 39;
                }
                mvwprintw(msg_win, msg_row++, 2, "%s: %s", 
                         shm->messages[i].sender, shm->messages[i].text);
            }
            last_message_count = shm->message_count;
            wrefresh(msg_win);
        }
        
        if (shm->client_count != last_client_count) {
            update_user_list();
            last_client_count = shm->client_count;
        }
        
        sem_post(&shm->mutex);
        usleep(100000);
    }
    return NULL;
}

int main() {
    signal(SIGINT, sigint_handler);

    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(1);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    msg_win = newwin(40, max_x - 42, 0, 0);
    user_win = newwin(40, 42, 0, max_x - 42);
    input_win = newwin(3, max_x, 40, 0);

    scrollok(msg_win, TRUE);
    box(msg_win, 0, 0);
    wattron(msg_win, A_REVERSE);
    mvwprintw(msg_win, 0, 2, "Messages");
    wattroff(msg_win, A_REVERSE);
    wrefresh(msg_win);

    box(user_win, 0, 0);
    wattron(user_win, A_REVERSE);
    mvwprintw(user_win, 0, 2, "Users");
    wattroff(user_win, A_REVERSE);
    wrefresh(user_win);

    box(input_win, 0, 0);
    wattron(input_win, A_REVERSE);
    mvwprintw(input_win, 0, 2, "Input");
    wattroff(input_win, A_REVERSE);
    wrefresh(input_win);

    key_t key = ftok("server.c", 1);
    shm_id = shmget(key, sizeof(SharedMemory), 0666);
    if (shm_id == -1) {
        endwin();
        perror("shmget");
        exit(1);
    }

    shm = (SharedMemory *)shmat(shm_id, NULL, 0);
    if (shm == (void *)-1) {
        endwin();
        perror("shmat");
        exit(1);
    }

    mvwprintw(input_win, 1, 2, "Your name: ");
    wrefresh(input_win);
    mvwgetnstr(input_win, 1, 14, username, USERNAME_LEN - 1);

    sem_wait(&shm->mutex);
    strncpy(shm->clients[shm->client_count].name, username, USERNAME_LEN);
    shm->clients[shm->client_count].active = 1;
    shm->client_count++;
    sem_post(&shm->mutex);

    werase(input_win);
    box(input_win, 0, 0);
    wattron(input_win, A_REVERSE);
    mvwprintw(input_win, 0, 2, "Input.   %s", username);
    wattroff(input_win, A_REVERSE);
    wrefresh(input_win);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receiver_thread, NULL);

    char input[MESSAGE_LEN];
    while (running) {
        wmove(input_win, 1, 2);
        wrefresh(input_win);
        wgetnstr(input_win, input, sizeof(input) - 1);

        if (strcmp(input, "exit") == 0) break;

        sem_wait(&shm->mutex);
        if (shm->message_count < MAX_MESSAGES) {
            strncpy(shm->messages[shm->message_count].sender, username, USERNAME_LEN);
            strncpy(shm->messages[shm->message_count].text, input, MESSAGE_LEN);
            shm->message_count++;
        } else {
            for (int i = 0; i < MAX_MESSAGES - 1; i++) {
                shm->messages[i] = shm->messages[i + 1];
            }
            strncpy(shm->messages[MAX_MESSAGES - 1].sender, username, USERNAME_LEN);
            strncpy(shm->messages[MAX_MESSAGES - 1].text, input, MESSAGE_LEN);
        }
        sem_post(&shm->mutex);

        werase(input_win);
        box(input_win, 0, 0);
        wattron(input_win, A_REVERSE);
        mvwprintw(input_win, 0, 2, "Input.   %s", username);
        wattroff(input_win, A_REVERSE);
        wrefresh(input_win);
    }

    running = 0;
    pthread_join(recv_thread, NULL);
    cleanup();
    return 0;
}