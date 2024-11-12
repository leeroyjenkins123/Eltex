#include <stdio.h>
#include <stdlib.h>

void new_third_byte(int *number, int new_byte)
{   
    printf("Before new third byte:\n");
    for (int i = 31; i >= 0; i--)
    {
        int bit = (*number >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");

    *number &= 0xFF00FFFF;
    new_byte <<= 16;
    *number |= new_byte;
    printf("After new third byte:\n");

    for (int i = 31; i >= 0; i--)
    {
        int bit = (*number >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

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

    int new_byte = 0;
    printf("Enter a positive decimal number that will be used to create a new byte [0,255]: ");
    scanf("%d", &new_byte);

    if (new_byte < 0 || new_byte > 255)
    {
        printf("Error! You entered a number that is not in the range [0,255].");
        return 0;
    }
    printf("\n");

    new_third_byte(&decimal_num, new_byte);

    printf("Number 'decimal_num' with new third byte: %d\n", decimal_num);

    return 0;
}

// 00000000 00000000 00000000 00000001
//   3          2       1       0
/*
01111111 11111111 11111111 11111111
01111111 00000000 11111111 11111111

*/