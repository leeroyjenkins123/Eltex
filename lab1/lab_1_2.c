#include <stdio.h>
#include <stdlib.h>

void dec_to_bin_forw(int number)
{
    printf("Negative number in forward code:\n");
    printf("1");
    number = -number;
    for (int i = 30; i >= 0; i--)
    {
        int bit = (number >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

void dec_to_bin_dop(int number)
{
    printf("Negative number in supplementary code:\n");
    for (int i = 31; i >= 0; i--)
    {
        int bit = (number >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

int main()
{
    int decimal_num = 0;

    printf("Enter a negative decimal number: ");
    scanf("%d", &decimal_num);

    if (decimal_num >= 0)
    {
        printf("Error! You entered a positive number.\n");
        return 0;
    }
    dec_to_bin_forw(decimal_num);
    dec_to_bin_dop(decimal_num);

    return 0;
}