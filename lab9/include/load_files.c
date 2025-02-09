#include "filemanager.h"

void free_dir_list(struct Panel *p)
{
    if (p->dirent)
    {
        for (int i = 0; i < p->count; i++)
        {
            free(p->dirent[i]);
        }
        free(p->dirent);
        p->dirent = NULL;
    }
}

void load_directory(struct Panel *p)
{
    free_dir_list(p);

    p->count = scandir(p->current_path, &p->dirent, NULL, alphasort);
    if (p->count < 0)
    {
        mvprintw(0, 0, "Ошибка scandir для %s: %s", p->current_path, strerror(errno));
        refresh();
        p->count = 0;
        p->dirent = NULL;
        return;
    }
    p->selected_item = 0; // Сбрасываем выбор
}