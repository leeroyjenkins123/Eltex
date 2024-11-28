#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
};

struct abonent mass_struct[100];
int count = 0;

void add_abonent()
{
    if (count >= 100)
    {
        printf("Массив переполнен. Невозможно добавить больше абонентов.\n");
        return;
    }

    struct abonent new_abonent;
    printf("Введите имя абонента (максимум 10 символов): ");
    scanf("%9s", new_abonent.name);
    while (getchar() != '\n')
        ;

    printf("Введите фамилию абонента (максимум 10 символов): ");
    scanf("%9s", new_abonent.second_name);
    while (getchar() != '\n')
        ;

    printf("Введите номер телефона абонента (максимум 10 символов. Маска - 9139999999): ");
    scanf("%9s", new_abonent.tel);
    while (getchar() != '\n')
        ;

    mass_struct[count++] = new_abonent;
    printf("Абонент добавлен.\n");
}

void print_menu()
{
    printf("\n\tМеню:\n");
    printf("1) Добавить абонента\n");
    printf("2) Удалить абонента\n");
    printf("3) Поиск абонентов по имени\n");
    printf("4) Вывод всех записей\n");
    printf("5) Выход\n");
    printf("Введите номер пункта меню: ");
}

void delete_abonent()
{
    char name_to_delete[10];
    printf("Введите имя абонента для удаления: ");
    scanf("%9s", name_to_delete);

    int found_to_delete = 0;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(mass_struct[i].name, name_to_delete) == 0)
        {
            for (int j = i; j < count - 1; j++)
            {
                mass_struct[j] = mass_struct[j + 1];
            }
            count--;
            found_to_delete=1;
            printf("Абонент %9s удален из справочника\n", name_to_delete);
            break;
        }
    }

    if (!found_to_delete)
    {
        printf("Абонент с именем %9s не найден в справочнике\n", name_to_delete);
    }
}

void print_abonents(){
    if(count ==0){
        printf("Справочник пуст\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        printf("Имя: %s, Фамилия: %s, Телефон: %s\n", mass_struct[i].name, mass_struct[i].second_name, mass_struct[i].tel);
    }
}

void find_abonent(){
    char name_to_find[10];
    printf("Введите имя абонента для поиска: ");
    scanf("%9s", name_to_find);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(mass_struct[i].name, name_to_find) == 0)
        {
            printf("Абонент %9s найден в справочнике\n", name_to_find);
            printf("Имя: %s, Фамилия: %s, Телефон: %s\n", mass_struct[i].name, mass_struct[i].second_name, mass_struct[i].tel);
            break;
        }
    }
}

int main()
{
    while (1)
    {
        int choice = 0;

        print_menu();
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            add_abonent();
            break;
        case 2:
            delete_abonent();
            break;
        case 3:
            find_abonent();
            break;
        case 4:
            print_abonents();
            break;
        case 5:
            printf("Выход из программы.\n");
            return 0;
            break;
        }
    }
    return 0;
}