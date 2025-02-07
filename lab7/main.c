#include <stdio.h>
#include "./include/calculator.h"

void print_menu()
{
    printf("\n\tMenu:\n");
    printf("1) Addition (a+b)\n");
    printf("2) Subtraction (a-b)\n");
    printf("3) Multiplication (a*b)\n");
    printf("4) Division (a/b)\n");
    printf("5) Exit\n");
    printf("Enter your choice: ");
}

void scan_numbers(int *a, int *b){
    printf("Enter your two decimal numbers separated by space: ");
    scanf("%d %d",a, b);
}

int main()
{
    printf("Welcome to simple calculator.\n");
    while (1)
    {
        int choice = 0, a=0,b=0;
        print_menu();

        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            scan_numbers(&a,&b);
            printf("%d + %d = %d",a,b,add(a,b));
            break;
        case 2:
            scan_numbers(&a,&b);
            printf("%d - %d = %d",a,b,sub(a,b));
            break;
        case 3:
            scan_numbers(&a,&b);
            printf("%d * %d = %d",a,b,mult(a,b));
            break;
        case 4:
            scan_numbers(&a,&b);
            printf("%d / %d = %d",a,b,div(a,b));
            break;
        case 5:
            printf("Exit from programm.\n");
            return 0;
            break;
        }
    }
    return 0;
}