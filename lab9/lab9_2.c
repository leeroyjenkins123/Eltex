#include "./include/filemanager.h"

int main()
{
    initscr();
    noecho();
    cbreak();
    start_color();
    keypad(stdscr, TRUE);
    curs_set(0);

    int height, width;
    getmaxyx(stdscr, height, width);

    WINDOW *win1 = newwin(height, width / 2, 0, 0);
    WINDOW *win2 = newwin(height, width / 2, 0, width / 2);

    struct Panel panels[2];

    init_panel(&panels[0], win1);
    init_panel(&panels[1], win2);

    panels[0].current_panel = 1;

    load_directory(&panels[0]);
    load_directory(&panels[1]);

    refresh();

    display_panel(&panels[0]);
    display_panel(&panels[1]);
    menu(&panels[0], &panels[1]);

    // getch();

    endwin();

    return 0;
}