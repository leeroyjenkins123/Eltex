#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n = 0;
    printf("Enter size of triangle matrix: ");

    if (scanf("%d", &n) != 1 || n <= 0)
    {
        printf("Invalid input! Please enter a positive integer.\n");
        return 1;
    }

    int arr[n][n];

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j >= n - 1 - i)
            {
                arr[i][j] = 1;
            }
            else
            {
                arr[i][j] = 0;
            }
        }
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