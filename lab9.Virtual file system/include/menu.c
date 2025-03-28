#include "filemanager.h"
void menu(struct Panel *left, struct Panel *right)
{
    int ch;

    while ((ch = getch()) != 'q')
    {
        switch (ch)
        {
        case KEY_UP:
            if (left->current_panel)
            {
                if (left->selected_item > 0)
                {
                    left->selected_item--;
                }
            }
            else
            {
                if (right->selected_item > 0)
                {
                    right->selected_item--;
                }
            }
            break;
        case KEY_DOWN:
            if (left->current_panel)
            {
                if (left->selected_item < left->count - 1)
                {
                    left->selected_item++;
                }
            }
            else
            {
                if (right->selected_item < right->count - 1)
                {
                    right->selected_item++;
                }
            }
            break;
        case '\t':
            if (left->current_panel)
            {
                init_pair(2, COLOR_WHITE, COLOR_BLACK);
                wbkgd(left->win, COLOR_PAIR(2));
                left->current_panel = 0;
                right->current_panel = 1;
            }
            else
            {
                init_pair(2, COLOR_WHITE, COLOR_BLACK);
                wbkgd(right->win, COLOR_PAIR(2));
                left->current_panel = 1;
                right->current_panel = 0;
            }
            break;
        case 10:
        case KEY_ENTER:
            if (left->current_panel)
            {
                if (left->count > 0)
                {
                    change_directory(left);
                }
            }
            else
            {
                if (right->count > 0)
                {
                    change_directory(right);
                }
            }
        default:
            break;
        }
        display_panel(left);
        display_panel(right);
    }
}