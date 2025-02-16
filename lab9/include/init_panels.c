#include "filemanager.h"

void init_panel(struct Panel *panel, WINDOW *win)
{
    panel->win = win;
    if (getcwd(panel->current_path, 1024) == NULL)
    {
        strcpy(panel->current_path, ".");
    }
    panel->dirent = NULL;
    panel->count = 0;
    panel->selected_item = 0;
    panel->current_panel = 0;
}