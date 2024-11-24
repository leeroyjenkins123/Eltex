#include <stdio.h>
#include <stdlib.h>

int main()
{
    int decimal_num = 0;

    printf("Enter a positive decimal number: ");
    scanf("%d", &decimal_num);

    if (decimal_num < 0)
    {
        printf("Error! You entered a negative number.\n");
        return 0;
    }

    char *ptr = (char *)&decimal_num;

    int new_byte;

    printf("Enter a positive decimal number that will be used to create a new byte [0,255]: ");
    scanf("%d", &new_byte);

    if (new_byte < 0 || new_byte > 255)
    {
        printf("Error! You entered a number that is not in the range [0,255].");
        return 0;
    }

    ptr[2] = new_byte;

    printf("Number 'decimal_num' with new third byte: %d\n", decimal_num);
    return 0;
}