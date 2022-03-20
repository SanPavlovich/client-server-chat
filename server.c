#include <sys/types.h>

#include <sys/wait.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <unistd.h>

#include <signal.h>

#include <sys/shm.h>

#include <sys/ipc.h>



#include <sys/msg.h>

#include <fcntl.h>

#include <sys/types.h>



#include "shmtest.c"



void handleTCPclient(int clientFd, int *fileD, int *toSonPipeFd, client *shmaddr, int clientCnt)

{

    printf("New client(%d) is connecting\n", clientFd);

    //связались с клиентом и теперь ждем от него имя:

    char clientName[30] = "";

    printf("Getting %d name... ", clientFd);

    recv(clientFd, clientName, sizeof(clientName), 0);

    clientName[strlen(clientName)] = '\0';

    printf("%s\n", clientName);

    

    int pipeReadFd = fileD[0];

    int pipeWriteFd = fileD[1];

  

    //передаем имя отцу, чтобы он, зная пид, поместил уже имя:

    write(pipeWriteFd, clientName, sizeof(clientName));

    //close(fileD[1]);

    printShmem(shmaddr, clientCnt);

    

    while(1 == 1)

    {

        //проверяю, присоединён ли клиент:

        /*char msgToClient[] = "are you online?";

        if(send(clientFd, msgToClient, strlen(msgToClient), 0) < 0)

        {

           //perror("Client died");

           printf("%s disconnected\n", clientName);

           break;

        }

        memset(msgToClient, 0, strlen(msgToClient));*/

   

        fd_set readset;

        FD_ZERO(&readset);

        FD_SET(clientFd, &readset);

        FD_SET(toSonPipeFd[0], &readset);

        int maxFd = clientFd > toSonPipeFd[0] ? clientFd : toSonPipeFd[0];

        printf("Before select, clientName: %s\n", clientName);

        select(maxFd+1, &readset, NULL, NULL, NULL);

        printf("After select, clientName: %s\n", clientName);

        if(FD_ISSET(clientFd, &readset))

        {

            char buf[257];

            memset(buf, 0, sizeof(buf)); //очистили буфер, чтобы при инициализации убрать оттуда мусор

            int clientMsgLen;        

             

            if((clientMsgLen = recv(clientFd, buf, 256, 0)) < 0)

            {

                printf("receive returned <0");

                break;

            }

            printf("Got command from client: %s, len of command: %d", buf, clientMsgLen);                



            if(clientMsgLen == 4 &&

               buf[0] == 'Q' && buf[1] == 'u' && buf[2] == 'i' && buf[3] == 't')

            {

                printf("%s leaves server\n", clientName);

                //принимаем прощальное сообщение от клиента:

                memset(buf, 0, sizeof(buf));

                recv(clientFd, buf, 256, 0);

                printf("%s's good-buy message: %s\n", clientName, buf);

                memset(buf, 0, sizeof(buf));

                 

                //удаляем из разделяемой памяти клиента:

                deleteName(shmaddr, clientCnt, clientName);

                //printShmem(shmaddr, clientCnt);

                break;

            }

            else if(buf[0] == 'U' && buf[1] == 's' && buf[2] == 'e' && buf[3] == 'r' &&

                    buf[4] == 's' && clientMsgLen == 5)

            {

                char onlineMsg[256] = "";

                memset(onlineMsg, 0, sizeof(onlineMsg));

                int online = cntOnline(shmaddr, clientCnt);

                //printf("online: %d\n", online);

                //printShmem(shmaddr, clientCnt);

                

                if(online == 1)

                {   

                    strcat(onlineMsg, "No one but you is online\n");

                    //printf("222 online: %d\n", online);

                }

                else if(online > 1)

                {

                    char onlineUsers[256] = "";

                    memset(onlineUsers, 0, sizeof(onlineUsers));

                    onlineNames(shmaddr, clientCnt, onlineUsers);

                     

                    strcat(onlineMsg, "'''''''''\n");

                    strcat(onlineMsg, "Online users:\n");

                    strcat(onlineMsg, onlineUsers);

                    strcat(onlineMsg, "\n'''''''''\n");

                }

                onlineMsg[strlen(onlineMsg)] = '\0';

                send(clientFd, onlineMsg, sizeof(onlineMsg), 0);

                //printf("Sent online Msg: %s\n", onlineMsg);

             }

             else if(buf[0] == 'P' && buf[1] == 'r' && buf[2] == 'i' && buf[3] == 'v' &&

                     buf[4] == 'a' && buf[5] == 't' && buf[6] == 'e' && buf[7] == '\0')

             {

                 printf("Getting private msg from client %s\n", clientName);

                 char privUser[30] = "";

                 memset(privUser, 0, sizeof(privUser));

                 char privMsg[256] = "";

                 memset(privMsg, 0, sizeof(privMsg));

                 recv(clientFd, privUser, sizeof(privUser), 0);

                 printf("Got private name: %s\n", privUser);

                 recv(clientFd, privMsg, sizeof(privMsg), 0);

                 printf("Got private msg: %s\n", privMsg);                 



                 char chatMsg[256] = "";

                 memset(chatMsg, 0, sizeof(chatMsg));

                 strcat(chatMsg, "(private) ");

                 strcat(chatMsg, clientName);

                 strcat(chatMsg, ": ");

                 strcat(chatMsg, privMsg);

                 strcat(chatMsg, "\n");



                 for(int i = 0; i < clientCnt; i++)

                 {

                    //printClient(&shmaddr[i]);

                    if((shmaddr[i].isEmpty == false) && (strcmp(shmaddr[i].Name, privUser) == 0))

                    {

                        printf("(private)Sending msg to %s\n", privUser);

                        printf("(private)Pipe write fd: %d, pid: %d\n", pipeWriteFd, shmaddr[i].pid);

                        write(pipeWriteFd, &shmaddr[i].pid, sizeof(shmaddr[i].pid));

                        write(pipeWriteFd, chatMsg, strlen(chatMsg));

                        printf("(private)Sent msg to %s\n", privUser);

                    }

                    else

                    {

                        printf("(private)not sending msg to %s\n", shmaddr[i].Name);

                    }

                 }



             }



             else

             {

                 printf("Got msg to another client from %s, msg: %s\n", clientName, buf);

                 //printf("%s: %s\n", clientName, buf);

                 char chatMsg[256] = "";

                 memset(chatMsg, 0, sizeof(chatMsg));

                 strcat(chatMsg, clientName);

                 strcat(chatMsg, ": ");

                 strcat(chatMsg, buf);

                 strcat(chatMsg, "\n");

                 chatMsg[strlen(chatMsg)] = '\0';

                  

                 //printShmem(shmaddr, clientCnt);            

                 

                 printf("Client name: %s\n", clientName);

                 for(int i = 0; i < clientCnt; i++)

                 {

                    //printClient(&shmaddr[i]);

                    if((shmaddr[i].isEmpty == false) && (strcmp(shmaddr[i].Name, clientName) != 0))

                    {

                        printf("Sending msg to %s\n", shmaddr[i].Name);

                        printf("Pipe write fd: %d, pid: %d\n", pipeWriteFd, shmaddr[i].pid);

                        write(pipeWriteFd, &shmaddr[i].pid, sizeof(shmaddr[i].pid));

                        write(pipeWriteFd, chatMsg, strlen(chatMsg));

                        printf("Sent msg to %s\n", shmaddr[i].Name);

                    }

                    else

                    {

                        printf("not sending msg to %s\n", shmaddr[i].Name);

                    }

                 }

                 memset(buf, 0, sizeof(buf)); //очистили буфер

             }

        }

        else if(FD_ISSET(toSonPipeFd[0], &readset))//обрабатываем сообщение от отца и отправляем клиенту:

        {

            printf("Getting msg to %s client:\n", clientName);

            char msgToClient[256] = "";

            memset(msgToClient, 0, sizeof(msgToClient));

            read(toSonPipeFd[0], msgToClient, sizeof(msgToClient));

            send(clientFd, msgToClient, strlen(msgToClient), 0);

            printf("Sent msg to another client: %s\n", msgToClient);

        }    

    }

    

    close(clientFd);

    exit(0);

}



int listenToServer()

{

    char serverCommand[256];

    memset(serverCommand, 0, sizeof(serverCommand));

    while(1 == 1)

    {

        //printf("input command: ");

        fgets(serverCommand, 256, stdin);

        serverCommand[strlen(serverCommand)] = '\0';

        if(serverCommand[0] == 'e' &&

           serverCommand[1] == 'x' &&

           serverCommand[2] == 'i' &&

           serverCommand[3] == 't')

            return -1;

        else

        {

            printf("There is no such command. Try again.\n");

            memset(serverCommand, 0, sizeof(serverCommand));

        }

    }

    return 0;

}





extern int errno;

void main ()

{

 char c;

 int serverFd, clientFd, fromlen;//fromlen - размер структуры адреса клиента в байтах

 FILE * fp; char hostname[64];

 struct hostent *hp;

 struct sockaddr_in sin, fromsin; //sin - структура, содержащая адрес сервера, fromsin - адрес клиента



// получаем хост-имя собственной ЭВМ:

 gethostname (hostname, sizeof (hostname));

 // определяем сетевой номер своей машины:

 if ((hp = gethostbyname (hostname)) == NULL) {

 fprintf (stderr, "%s: unknown host.\n", hostname);

 exit (1);

 }

 // получаем свой сокет-дескриптор:

 if ((serverFd = socket (AF_INET, SOCK_STREAM, 0)) < 0) { perror ("client: socket"); exit (1);

 }

 // создаем адрес, который свяжется с сокетом:

 sin.sin_family = AF_INET;

 sin.sin_port = htons (1234);

 // копируем сетевой номер: memcpy ( &sin.sin_addr, hp->h_addr, hp->h_length);

 // связываем адрес с сокетом

 if ( bind ( serverFd, (struct sockaddr *)&sin, sizeof (sin)) < 0 ){

 perror ("server: bind");

 exit (1);

 }

 // слушаем запросы на сокет

 if ( listen ( serverFd, 5) < 0 ) {

 perror ("server: listen");

 exit (1);

 }

 // связываемся с клиентом через неименованный сокет с дескриптором d1;

 // после установления связи адрес клиента содержится в структуре fromsin

 fromlen = sizeof fromsin;

 /*if((clientFd = accept ( serverFd, (struct sockaddr *)&fromsin, &fromlen)) < 0){ perror ("server: accept");

 exit (1);

 }*/



 //список клиентов будем хранить в разделяемой памяти:

 int clientCnt = 5;

 key_t key;

 key = ftok("usr/bin", 's');

 int shmid;

 client *shmaddr;

 shmid = shmget(key, clientCnt*(sizeof(client)), 0666|IPC_CREAT);

 shmaddr = shmat(shmid, NULL, 0);

 initShmem(shmaddr, clientCnt);

 

 //считываем ввод с сервера:

 pid_t servPid;

 if((servPid = fork()) < 0)

    printf("can't read server commands\n");

 else if(servPid == 0)

 {

    if(listenToServer() == -1)

    {

        printf("The server is closing\n");

        close(serverFd);

        

        shmdt(shmaddr);

        shmctl(shmid, IPC_RMID, NULL);

        kill(servPid, SIGINT);

        exit(0);

    }    

 }



 // читаем сообщения клиента, пишем клиенту:

 while(1 == 1)

 {

    fd_set readset;

    FD_ZERO(&readset);

    FD_SET(serverFd, &readset);



    int maxFd = serverFd;

    for(int i = 0; i < clientFd; i++)

    {

        if(shmaddr[i].isEmpty == false)

        {

            maxFd = maxFd < shmaddr[i].pipeReadFd ? shmaddr[i].pipeReadFd : maxFd;

            FD_SET(shmaddr[i].pipeReadFd, &readset);

        }

    }

    select(maxFd+1, &readset, NULL, NULL, NULL);

    

    if(FD_ISSET(serverFd, &readset))

    {

        if((clientFd = accept(serverFd, (struct sockaddr *)&fromsin, &fromlen)) < 0)

        {

            perror("server didn't accept client\n");

            //exit(1);

        }

        else

        {

            int fd[2];

            int toSonPipeFd[2];

            //fd[0] - read

            //fd[1] - write

            pipe(fd);

            pipe(toSonPipeFd);

    

            pid_t sonProcessID;

             

            if((sonProcessID = fork()) < 0)

                 printf("son process fork failed for client: %d\n", clientFd);

            else if(sonProcessID == 0)

                handleTCPclient(clientFd, fd, toSonPipeFd, shmaddr, clientCnt);

            else if(sonProcessID > 0);

            {

                writePid(shmaddr, clientCnt, sonProcessID);

                writeFd(shmaddr, clientCnt, sonProcessID, clientFd, fd[0], fd[1], toSonPipeFd);

                           

                char clientName[30];

                //close(fd[1]);

                read(fd[0], clientName, sizeof(clientName));

                //close(fd[0]);

                writeName(shmaddr, clientCnt, sonProcessID, clientName);

            

                //printShmem(shmaddr, clientCnt);            

            }

        }

     }

     else //получаем сообщение от сына и отправляем сообщение другим сыновьям через отца

     {

        printf("Working with pipe in father:\n");

        for(int i = 0; i < clientCnt; i++)

        {

            if(FD_ISSET(shmaddr[i].pipeReadFd, &readset))

            {

                char sonMsg[256] = "";

                memset(sonMsg, 0, sizeof(sonMsg));

                int targetSonPid;

                printf("Reading msg from son, client name: %s\n", shmaddr[i].Name);

                printf("pipeReadFd: %d, pipeWriteFd: %d\n", shmaddr[i].pipeReadFd, shmaddr[i].pipeWriteFd);

                read(shmaddr[i].pipeReadFd, &targetSonPid, sizeof(targetSonPid));

                printf("Read pid: %d\n", targetSonPid);

                read(shmaddr[i].pipeReadFd, sonMsg, sizeof(sonMsg));

                printf("Read son msg: %s\n", sonMsg);

                for(int j = 0; j < clientCnt; j++)

                {

                    if(shmaddr[j].pid == targetSonPid)

                    {

                        printf("writing msg to son of client: %s\n", shmaddr[j].Name);

                        write(shmaddr[j].toSonPipeFd[1], sonMsg, strlen(sonMsg));

                        break;

                    }

                }

            }

        }

     }

    

 }

 

 shmdt(shmaddr);

 shmctl(shmid, IPC_RMID, NULL);

 close (serverFd);

 exit (0);

}

