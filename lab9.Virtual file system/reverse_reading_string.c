#include <stdio.h>
#include <stdlib.h>

int main()
{
    char text_to_write[] = "String from file";

    FILE *file;
    file = fopen("output.txt", "w");

    fprintf(file, "%s", text_to_write);
    fclose(file);

    file = fopen("output.txt", "r");

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    char reverse[size];
    fscanf(file, "%s", reverse);

    for (int i = size - 1; i >= 0; i--)
    {
        fseek(file, i, SEEK_SET);
        reverse[size - i-1] = fgetc(file);
    }
    fclose(file);

    printf("%s\n", reverse);

    return 0;
}