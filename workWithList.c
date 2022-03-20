//#include "namelist.h"

#include <stdio.h>

#include <string.h>

#include <stdlib.h>





typedef struct Node{

    int data;

    char userName[30];

    struct Node *next;

} Node;



void init(Node* p, int number, char *name)

{

    if(strlen(name) >= 30)

    {

        printf("error: long name\n");

        return;

    }

    p->data = number;

    int i = 0;

    while(name[i] != '\0')

    {

        p->userName[i] = name[i];

        i++;

    }

    p->userName[i] = '\0';

}



void push(Node **head, int number, char *name)

{

    Node *tmp = (Node*)malloc(sizeof(Node));

    init(tmp, number, name);

    tmp->next = (*head);

    (*head) = tmp;

}



void pushBack(Node **head, int number, char *name)

{

    if(*head == NULL)

    {

       // printf("in push ");

        push(head, number, name);

    }

    else

    {

        Node *p = *head;

        while(p->next != NULL)

            p = p->next;

        Node *tmp = (Node*)malloc(sizeof(Node));

        tmp->next = NULL;

        init(tmp, number, name);

        p->next = tmp;

    }

}



void deleteNum(Node **head, int number)

{

    if(*head != NULL)

    {

        if((*head)->data == number)

        {

            if((*head)->next == NULL)

            {

                (*head) = NULL;

                return;

            }

            //printf("bruh ");

            Node *p = *head;

            (*head) = (*head)->next;

            p = NULL;

            free(p);

        }

        

        Node *p = *head;

        Node *q = p;

        while(p->next != NULL)

        {

            if(p->data == number)

                break;

            q = p;

            p = p->next;

        }

        if(p->data != number)

            return;

        q->next = q->next->next;

        p = NULL;

    }

}



Node* popNum(Node **head, int number)

{

    if(*head != NULL)

    {

        if((*head)->data == number)

        {

            Node *p = *head;

            if((*head)->next == NULL)

            {

                (*head) = NULL;

                return p;

            }

           // printf("bruh ");

            (*head) = (*head)->next;

            return p;

        }

        

        Node *p = *head;

        Node *q = p;

        while(p->next != NULL)

        {

            if(p->data == number)

                break;

            q = p;

            p = p->next;

        }

        if(p->data != number)

            return NULL;

        q->next = q->next->next;

        return p;

        //p = NULL;

    }



}



Node* popName(Node **head, char *name)

{

    if(*head != NULL)

    {

        if(strcmp((*head)->userName, name) == 0)

        {

            Node *p = *head;

            if((*head)->next == NULL)

            {

                (*head) = NULL;

                return p;

            }

           // printf("bruh ");

            (*head) = (*head)->next;

            return p;

        }

        

        Node *p = *head;

        Node *q = p;

        while(p->next != NULL)

        {

            if(strcmp((*head)->userName, name) == 0)

                break;

            q = p;

            p = p->next;

        }

        if(strcmp((*head)->userName, name) != 0)

            return NULL;

        q->next = q->next->next;

        return p;

        //p = NULL;

    }



}



void addName(Node *head, int number, char *name)

{

    Node *p = head;

    while(p->next != NULL)

    {

        printf("1 ");

        if(p->data == number)

            break;

        p = p->next;

    }

    if(p->data != number)

        return;

    init(p, number, name);

}



int lenList(Node *head)

{

    if(head == NULL)

        return 0;

    else

    {

        int len = 0;

        Node *p = head;

        while(p != NULL)

        {

            len++;

            p = p->next;

        }

        return len;

    }

}



Node *takeByOrder(Node **head, int order)

{

    int i = 0;

    Node *p = *head;

    while(i < order && p != NULL)

    {

        i++;

        p = p->next;

    }

    if(p == NULL)

        return NULL;

    return popNum(head, p->data);

}



void printPrivates(Node *head)

{

    if(head != NULL)

    {

        Node *p = head;

        printf("'''''''''\n");

        printf("You sent private messages to:\n");

        while(p->next != NULL)  

        {

            printf("%s\n", p->userName);

            p = p->next;

        }

        printf("%s\n", p->userName);

        printf("'''''''''\n");

    }

    else

        printf("You didn't send any private messages\n");

}



void printList(Node *head)

{

    if(head != NULL)

    {

        Node *p = head;

        while(p->next != NULL)

        {

            printf("pid: %d, userName: %s\n", p->data, p->userName);

            p = p->next;

        }

        printf("pid: %d, userName: %s\n", p->data, p->userName);

    }

    else

        printf("List is empty\n");

}



/*int main()

{

    Node* List = NULL;

    //char Name[30];

    //scanf("%s", Name);

    //Node *p = (Node*)malloc(sizeof(Node*));

    //init(p, 5, "Sanches");

    //printf("data: %d, userName: %s", p->data, p->userName);



    pushBack(&List, 3145, "Nastya");   

    pushBack(&List, 2189, ".");

    pushBack(&List, 1234, "Ball");

    //pushBack(&List, 45);

    //pushBack(&List, 14);

    printList(List);

    //addName(List, 2189, "Sanya");

    Node* num = takeByOrder(&List, 0);

    printf("\n");

    //printf("length: %d\n", lenList(List));

    printList(List);

    printf("we order: 1, name is: %s", num->userName);

}*/

