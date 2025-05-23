#include <stdio.h>
#include <stdlib.h>

void dec_to_bin(int number)
{
    for (int i = 31; i >= 0; i--)
    {
        int bit = (number >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

int main()
{
    int decimal_num=0;

    printf("Enter a positive decimal number: ");
    scanf("%d", &decimal_num);

    if (decimal_num < 0)
    {
        printf("Error! You entered a negative number.\n");
        return 0;
    }
    dec_to_bin(decimal_num);

    return 0;
}