#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *find_substring(const char *first, const char *second)
{
    while (*first)
    {
        char *f = (char *)first;
        char *s = (char *)second;

        while (*f && *s && *f == *s)
        {
            f++;
            s++;
        }

        if (*s == '\0')
        {
            return (char *)first;
        }
        first++;
    }
    return NULL;
}

int main()
{
    char first[100], second[100];

    printf("Enter line where you will find another line: ");
    fgets(first, sizeof(first), stdin);
    first[strcspn(first, "\n")] = '\0';

    printf("Enter line that need to be found in first line: ");
    fgets(second, sizeof(second), stdin);
    second[strcspn(second, "\n")] = '\0';

    char *result = find_substring(first, second);

    if (result == NULL)
    {
        printf("Substring not found\n");
    }
    else
    {
        printf("Substring found at position: %ld\n", result-first);
    }

    return 0;
}