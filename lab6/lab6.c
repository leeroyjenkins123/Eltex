#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
    struct abonent *next, *prev;
};

struct abonent *head = NULL;

void add_abonent()
{
    struct abonent *new_abonent = (struct abonent *)malloc(sizeof(struct abonent));
    printf("Введите имя абонента (максимум 10 символов): ");
    scanf("%9s", new_abonent->name);
    while (getchar() != '\n')
        ;

    printf("Введите фамилию абонента (максимум 10 символов): ");
    scanf("%9s", new_abonent->second_name);
    while (getchar() != '\n')
        ;

    printf("Введите номер телефона абонента (максимум 10 символов. Маска - 9139999999): ");
    scanf("%9s", new_abonent->tel);
    while (getchar() != '\n')
        ;

    if (head == NULL)
    {
        head = new_abonent;
        head->next = NULL;
        head->prev = NULL;
    }
    else
    {
        struct abonent *temp = head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_abonent;
        new_abonent->prev = temp;
        new_abonent->next = NULL;
    }

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

    struct abonent *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->name, name_to_delete) == 0)
        {
            if (temp == head)
            {
                head = temp->next;
                if (head != NULL)
                {
                    head->prev = NULL;
                }
            }
            else
            {
                temp->prev->next = temp->next;
                if (temp->next != NULL)
                {
                    temp->next->prev = temp->prev;
                }
            }
            free(temp);
            printf("Абонент %9s удален из справочника\n", name_to_delete);
            found_to_delete = 1;
            break;
        }
        temp = temp->next;
    }

    if (!found_to_delete)
    {
        printf("Абонент с именем %9s не найден в справочнике\n", name_to_delete);
    }
}

void print_abonents(){
    struct abonent *temp = head;
    while (temp != NULL)
    {
        printf("Имя: %s, Фамилия: %s, Телефон: %s\n", temp->name, temp->second_name, temp->tel);
        temp = temp->next;
    }
}

void find_abonent(){
    char name_to_find[10];
    printf("Введите имя абонента для поиска: ");
    scanf("%9s", name_to_find);

    struct abonent *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->name, name_to_find) == 0)
        {
            printf("Абонент %9s найден в справочнике\n", name_to_find);
            printf("Имя: %s, Фамилия: %s, Телефон: %s\n", temp->name, temp->second_name, temp->tel);
            break;
        }
        temp = temp->next;
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