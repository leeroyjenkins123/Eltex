#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define REGISTER 1
#define MESSAGE 2
#define UNREGISTER 3
#define NEW_USER 4
#define CHAT_MSG 5
#define USER_LEFT 6

struct msg_buffer
{
	long mtype;
	char data[288];
};

int server_qid, client_qid;
char username[32];
WINDOW *msg_win, *user_win, *input_win;
int row = 1;
int user_row = 1;
int running = 1;
char user_list[100][32];
int user_count = 0;

void cleanup()
{
	struct msg_buffer msg;
	msg.mtype = UNREGISTER;
	memcpy(msg.data, username, 32);
	memcpy(msg.data + 32, &client_qid, sizeof(int));
	msgsnd(server_qid, &msg, sizeof(msg.data), 0);
	msgctl(client_qid, IPC_RMID, NULL);
	endwin();
}

void sigint_handler(int sig)
{
	running = 0;
}

void *receiver_thread(void *arg)
{
	struct msg_buffer msg;
	while (running)
	{
		if (msgrcv(client_qid, &msg, sizeof(msg.data), 0, IPC_NOWAIT) != -1)
		{
			if (msg.mtype == CHAT_MSG)
			{
				char sender[32], text[256];
				memcpy(sender, msg.data, 32);
				memcpy(text, msg.data + 32, 256);

				if (row >= 40)
				{
					wscrl(msg_win, 1);
					row = 39;
				}
				mvwprintw(msg_win, row++, 2, "%s: %s", sender, text);
				wrefresh(msg_win);
			}
			else if (msg.mtype == NEW_USER)
			{
				char new_user[32];
				memcpy(new_user, msg.data, 32);

				strncpy(user_list[user_count], new_user, 32);
				user_count++;

				if (user_row >= 40)
				{
					wscrl(user_win, 1);
					user_row = 39;
				}
				mvwprintw(msg_win, row++, 2, "New connection: %s", new_user);
				wrefresh(msg_win);
				mvwprintw(user_win, user_row++, 2, "- %s", new_user);
				wrefresh(user_win);
			}
			else if (msg.mtype == USER_LEFT)
			{
				char left_user[32];
				memcpy(left_user, msg.data, 32);

				for (int i = 0; i < user_count; i++)
				{
					if (strcmp(user_list[i], left_user) == 0)
					{
						for (int j = i; j < user_count - 1; j++)
						{
							strcpy(user_list[j], user_list[j + 1]);
						}
						user_count--;
						break;
					}
				}

				mvwprintw(msg_win, row++, 2, "Use %s left", left_user);
				wrefresh(msg_win);

				werase(user_win);
				box(user_win, 0, 0);
				wattron(user_win, A_REVERSE);
				mvwprintw(user_win, 0, 2, "Users");
				wattroff(user_win, A_REVERSE);

				user_row = 1;
				for (int i = 0; i < user_count; i++)
				{
					mvwprintw(user_win, user_row++, 2, "- %s", user_list[i]);
				}
				wrefresh(user_win);
			}
		}
		else
		{
			usleep(10000);
		}
	}
	return NULL;
}

int main()
{
	signal(SIGINT, sigint_handler);

	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(1);

	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	msg_win = newwin(42, max_x - 42, 0, 0);
	user_win = newwin(42, 42, 0, max_x - 42);
	input_win = newwin(3, max_x, 42, 0);

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

	client_qid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

	key_t server_key = ftok("server.c", 1);
	server_qid = msgget(server_key, 0666);

	pthread_t recv_thread;
	pthread_create(&recv_thread, NULL, receiver_thread, NULL);

	mvwprintw(input_win, 1, 2, "Your name: ");
	wrefresh(input_win);
	mvwgetnstr(input_win, 1, 14, username, 31);

	struct msg_buffer reg_msg;
	reg_msg.mtype = REGISTER;
	memcpy(reg_msg.data, username, 32);
	memcpy(reg_msg.data + 32, &client_qid, sizeof(int));
	msgsnd(server_qid, &reg_msg, sizeof(reg_msg.data), 0);

	werase(input_win);
	box(input_win, 0, 0);
	wattron(input_win, A_REVERSE);
	mvwprintw(input_win, 0, 2, "Input.   %s", username);
	wattroff(input_win, A_REVERSE);
	wrefresh(input_win);


	char input[256];
	while (running)
	{
		wmove(input_win, 1, 2);
		wrefresh(input_win);

		wgetnstr(input_win, input, sizeof(input) - 1);

		if (strcmp(input, "exit") == 0)
			break;

		struct msg_buffer msg;
		msg.mtype = MESSAGE;
		memcpy(msg.data, username, 32);
		memcpy(msg.data + 32, input, 256);
		msgsnd(server_qid, &msg, sizeof(msg.data), 0);

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
