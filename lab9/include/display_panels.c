#include "filemanager.h"

void display_panel(struct Panel *panel)
{
    werase(panel->win);
    box(panel->win, 0, 0);

    if (panel->current_panel)
    {
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        wbkgd(panel->win, COLOR_PAIR(1));
        wattron(panel->win, A_REVERSE);
    }

    mvwprintw(panel->win, 0, 2, "%s", panel->current_path);

    if (panel->current_panel)
    {
        wattroff(panel->win, A_REVERSE);
    }

    int width, height;
    getmaxyx(panel->win, height, width);

    int limits = height - 2, start = 0;

    if (panel->selected_item >= limits)
    {
        start = panel->selected_item - limits + 1;
    }

    for (int i = start; i < panel->count && i < start + limits; i++)
    {
        if (i == panel->selected_item && panel->current_panel)
        {
            wattron(panel->win, A_REVERSE);
        }
        if(panel->dirent[i]->d_type == DT_DIR){
            mvwprintw(panel->win, i - start + 1, 1, "%s/", panel->dirent[i]->d_name);
        }
        else{
            mvwprintw(panel->win, i - start + 1, 1, "%s", panel->dirent[i]->d_name);
        }
        
        wattroff(panel->win, A_REVERSE);
    }

    wrefresh(panel->win);
}