#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n = 0;
    printf("Enter size of 'snail' matrix: ");

    if (scanf("%d", &n) != 1 || n <= 0)
    {
        printf("Invalid input! Please enter a positive integer.\n");
        return 1;
    }

    int arr[n][n];

    int koef = 1;
    int top = 0, bot = n - 1, left = 0, right = n - 1;

    while (koef <= n * n)
    {
        for (int i = left; i <= right; i++)
        {
            arr[top][i] = koef++;
        }
        top++;

        for (int i = top; i <= bot; i++)
        {
            arr[i][right] = koef++;
        }

        right--;
        for (int i = right; i >= left; i--)
        {
            arr[bot][i] = koef;
            koef++;
        }
        bot--;

        for (int i = bot; i >= top; i--)
        {
            arr[i][left] = koef;
            koef++;
        }
        
        left++;
    }

    printf("Triangle matrix:\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%2d ", arr[i][j]);
        }
        printf("\n");
    }

    return 0;
}