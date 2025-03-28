#include <stdio.h>
#include "./include/calculator.h"
#include <ncurses.h>

void print_menu(WINDOW *win, int highlight)
{
    const char *choices[] = {
        "Addition (a+b)",
        "Subtraction (a-b)",
        "Multiplication (a*b)",
        "Division (a/b)",
        "Exit"};
    int n_choices = sizeof(choices) / sizeof(choices[0]);

    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "Menu:");
    for (int i = 0; i < n_choices; i++)
    {
        if (i == highlight)
        {
            wattron(win, A_REVERSE);
        }
        mvwprintw(win, i + 3, 2, "%d) %s", i + 1, choices[i]);
        wattroff(win, A_REVERSE);
    }
    wrefresh(win);
}

void scan_numbers(WINDOW *win, int *a, int *b)
{
    wrefresh(win);
    mvwprintw(win, 9, 2,"Enter your two decimal numbers:");
    echo();
    mvwscanw(win, 10, 2,"%d %d", a, b);
    noecho();
}

int main()
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    int height = 15, width = 50, start_y = 5, start_x = 5;

    WINDOW *win = newwin(height, width, start_y, start_x);
    keypad(win, TRUE);

    refresh();
    box(win, 0, 0);

    wattron(win, A_REVERSE);
    mvwprintw(win, 0, 10, "Welcome to simple calculator");
    wattroff(win, A_REVERSE);

    int choice = 0, highlight = 0;
    while (1)
    {
        print_menu(win, highlight);
        int choice = wgetch(win);

        switch(choice){
            case KEY_UP:
            highlight = (highlight == 0) ? 4 : highlight-1;
            break;
            case KEY_DOWN:
            highlight = (highlight == 4) ? 0 : highlight + 1;
            break;
            case 10:
            if(highlight == 4){
                endwin();
                return 0;
            } 
            int a=0, b=0;
            scan_numbers(win, &a, &b);
            int result;
            switch (highlight)
            {
            case 0:
                result = add(a,b);
                break;
            case 1:
                result = sub(a,b);
                break;
            case 2:
                result = mult(a,b);
                break;
            case 3:
                result = div(a,b);
                break;
            default:
                break;
            }
            mvwprintw(win, 12,2, "Result: %d", result);
            wrefresh(win);
            wgetch(win);
            break;
        }
    }
    endwin();
    return 0;
}