#include <ncurses.h>
#include <panel.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

struct Panel
{
    WINDOW *win;
    char current_path[1024];
    struct dirent **dirent;
    int count;
    int selected_item;
    int current_panel;
};

void menu(struct Panel *left, struct Panel *right);
void init_panel(struct Panel *panel, WINDOW *win);
void free_dir_list(struct Panel *p);
void load_directory(struct Panel *p);
void display_panel(struct Panel *panel);
void change_directory(struct Panel *panel);