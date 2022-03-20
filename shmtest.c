#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/shm.h>

#include <sys/wait.h>

#include <stdbool.h>

#include <string.h>

#include <sys/ipc.h>

#include <sys/sem.h>



typedef struct {

  bool isEmpty;  

  int pid;

  char Name[30];

  int clientFd;

  int pipeReadFd;

  int pipeWriteFd;

  int toSonPipeFd[2];

} client;



void initNames(client *X, int clientCnt)

{

    for(int i = 0; i < clientCnt; i++)

        (*X).isEmpty = true;

}



void printClient(client *X)

{

    if(X != NULL)

    {

        printf("isEmpty: %d\n", (*X).isEmpty);

        printf("pid: %d\n", (*X).pid);

        printf("Name: %s\n", (*X).Name);

        printf("Client fd: %d\n", (*X).clientFd);

        printf("Pipe read fd: %d\n", (*X).pipeReadFd);

        printf("Pipe write fd: %d\n", (*X).pipeWriteFd);

        printf("to son pipe: fd[0] = %d, fd[1] = %d\n",(*X).toSonPipeFd[0], (*X).toSonPipeFd[1]);

        printf("\n");

    }

}



void printClients(client *X, int clientCnt)

{

    if(X != NULL)

    {

        for(int i = 0; i < clientCnt; i++)

        if(X[i].isEmpty == false)

            printClient(&(X[i]));

    }

    else

        printf("Nobody to print\n");

}



void clientCpy(client *memaddr, client *x)

{

    (*memaddr).isEmpty = (*x).isEmpty;

    (*memaddr).pid = (*x).pid;

    strcpy((*memaddr).Name, (*x).Name);

    (*memaddr).clientFd = (*x).clientFd;

    (*memaddr).pipeReadFd = (*x).pipeReadFd;

    (*memaddr).pipeWriteFd = (*x).pipeWriteFd;

    (*memaddr).toSonPipeFd[0] = (*x).toSonPipeFd[0];

    (*memaddr).toSonPipeFd[1] = (*x).toSonPipeFd[1];

}



void printShmem(client *memaddr, int clientCnt)

{

    for(int i = 0; i < clientCnt; i++)

    {

        printf("print %d\n", i);

        printClient(&(memaddr[i]));

        printf("print %d\n", i);

    }

}



int cntOnline(client *memaddr, int clientCnt)

{

    int onlineCnt = 0;

    for(int i = 0; i < clientCnt; i++)

    {

        if(memaddr[i].isEmpty == false)

            onlineCnt++;

    }

    return onlineCnt;

}



void onlineNames(client *memaddr, int clientCnt, char *names)

{

    char Names[256] = "";

    for(int i = 0; i < clientCnt; i++)

    {

        if(memaddr[i].isEmpty == false)

        {

            strcat(Names, memaddr[i].Name);

            strcat(Names, " ");

           //printf("i: %d, Names: %s\n", i, Names);

        }

    }

    strcpy(names, Names);

    names[strlen(names)] = '\0';

    return;

}



void initShmem(client *memaddr, int clientCnt)

{

    for(int i = 0; i < clientCnt; i++)

    {

        //printf("%d\n", i);

        memaddr[i].isEmpty = true;

        memaddr[i].pid = 0;

        memaddr[i].Name[0] = '\0';

        memaddr[i].clientFd = 0;

        memaddr[i].pipeReadFd = 0;

        memaddr[i].pipeWriteFd = 0;

        memaddr[i].toSonPipeFd[0] = 0;

        memaddr[i].toSonPipeFd[1] = 0;

        //printf("%d\n", i);

    }

}



void writePid(client *memaddr, int clientCnt, int number)

{

    for(int i = 0; i < clientCnt; i++)

    {

        if(memaddr[i].isEmpty == true)

        {

            memaddr[i].isEmpty = false;

            memaddr[i].pid = number;

            break;

        }

    }

}



void writeName(client *memaddr, int clientCnt, int number, char *name)

{

    for(int i = 0; i < clientCnt; i++)

    {

        if(memaddr[i].pid == number)

        {

            strcpy(memaddr[i].Name, name);

            break;

        }

    }

}



void writeFd(client *memaddr, int clientCnt, int number, int clientFd, int readFd, int writeFd,

            int *toSonPipeFd)

{

    for(int i = 0; i < clientCnt; i++)

    {

        if(memaddr[i].pid == number)

        {

            memaddr[i].clientFd = clientFd;

            memaddr[i].pipeReadFd = readFd;

            memaddr[i].pipeWriteFd = writeFd;

            memaddr[i].toSonPipeFd[0] = toSonPipeFd[0];

            memaddr[i].toSonPipeFd[1] = toSonPipeFd[1];

            break;

        }

    }

}



void deleteName(client *memaddr, int clientCnt, char *name)

{

    for(int i = 0; i < clientCnt; i++)

    {   

        if(strcmp(memaddr[i].Name, name) == 0)

        {

            memaddr[i].isEmpty = true;

            memaddr[i].pid = 0;

            memaddr[i].Name[0] = '\0';

            memaddr[i].clientFd = 0;

            memaddr[i].pipeReadFd = 0;

            memaddr[i].pipeWriteFd = 0;

            memaddr[i].toSonPipeFd[0] = 0;

            memaddr[i].toSonPipeFd[1] = 0;

            break;

        }

    }

}



/*int main()

{

    printf("5");

    int clientCnt = 5;

    key_t key;

    key = ftok("usr/bin", 's');

    int shmid;

    client *shmaddr;

    shmid = shmget(key, (clientCnt)*(sizeof(client)), 0666|IPC_CREAT);

    shmaddr = shmat(shmid, NULL, 0);

    printf("shmid: %d shmaddr %p\n", shmid, shmaddr);

    initShmem(shmaddr, clientCnt);

    printShmem(shmaddr, clientCnt);    

    

    pid_t pid;

    if((pid = fork()) < 0)

        printf("fork error\n");

    else if(pid == 0)

    {

        printf("Son print:\n");

        writePid(shmaddr, clientCnt, 15);

        writeName(shmaddr, clientCnt, 15, "Sanya");

        writeFd(shmaddr, clientCnt, 15, 1445);

        writePid(shmaddr, clientCnt, 16);

        writeName(shmaddr, clientCnt, 16, "Vasya");

        writeFd(shmaddr, clientCnt, 16, 1739);

        writePid(shmaddr, clientCnt, 43);

        writeName(shmaddr, clientCnt, 43, "Dima");

        writeFd(shmaddr, clientCnt, 43, 1290);

        //char names[256];

        //onlineNames(shmaddr, clientCnt, names);

        //printf("Online: %s", names);

        //printf("\n");

        printShmem(shmaddr, clientCnt);

        exit(0);

        

    }   

    else if(pid > 0)

    {

        wait(0);

        deleteName(shmaddr, clientCnt, "Dima");

        printShmem(shmaddr, clientCnt);

    }

   // printf("father print:\n");

   // printShmem(shmaddr, clientCnt);



    shmdt(shmaddr);

    shmctl(shmid, IPC_RMID, NULL);

}*/

