#include <stdio.h>
#include <stdlib.h>

void print_array(int arr[], int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main()
{
    int n = 0;
    printf("Enter size of array: ");

    if (scanf("%d", &n) != 1 || n <= 0)
    {
        printf("Invalid input! Please enter a positive integer.\n");
        return 1;
    }

    int arr[n];
    
    for (int i = 0; i < n; i++)
    {
        arr[i] = i + 1;
    }
    print_array(arr,n);


    for (int i = 0; i < n/2; i++)
    {
        int temp = arr[i];
        arr[i] = arr[n - 1 - i];
        arr[n - 1 - i] = temp;
    }

    printf("Reverse array:\n");
    print_array(arr,n);

    return 0;
}