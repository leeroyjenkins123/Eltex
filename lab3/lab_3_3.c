#include <stdio.h>
#include <stdlib.h>

int main()
{
    int array[10];

    for (int i = 0; i < 10; i++)
    {
        array[i] = (i + 1) * 5;
    }

    int *ptr = array;

    printf("Array printed with pointer:\n");

    while(ptr < array+10){
        printf("%d ", *ptr);
        ptr++;
    }
    printf("\n");

    return 0;
}