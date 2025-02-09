#include "filemanager.h"

void change_directory(struct Panel *panel)
{
    struct dirent *entry = panel->dirent[panel->selected_item];
    char new_path[1024];

    if (strcmp(entry->d_name, ".") == 0)
    {
        return;
    }
    else if (strcmp(entry->d_name, "..") == 0)
    {
        char *last_slash = strrchr(panel->current_path, '/');
        if (last_slash && last_slash != panel->current_path)
        {
            *last_slash = '\0';
        }
        else
        {
            strcpy(panel->current_path, "/");
        }
    }
    else
    {
        if (strcmp(panel->current_path, "/") == 0)
            snprintf(new_path, sizeof(new_path), "/%s", entry->d_name);
        else
            snprintf(new_path, sizeof(new_path), "%s/%s", panel->current_path, entry->d_name);

        struct stat st;
        if (stat(new_path, &st) == 0 && S_ISDIR(st.st_mode))
        {
            strncpy(panel->current_path, new_path, 1024);
            panel->current_path[1024 - 1] = '\0';
        }
        else
        {
        }
    }
    load_directory(panel);
}