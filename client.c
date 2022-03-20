#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <stdio.h>

#include <string.h>

#include <unistd.h>

#include <stdlib.h>

#include <fcntl.h>

#include <sys/time.h>

#include <sys/types.h>

#include <signal.h>

#include "test1.c"



#define SOCKET_STDIN 0



void printCommands()

{

    printf("'''''''''\n");

    printf("Help - print commands\n");

    printf("Quit - to leave server\n");

    printf("Users - to get names of users, which are online\n");

    printf("Private - to send private message to somebody(you should enter nickname)\n");

    printf("Privates - the names of the users to whom you sent private messages\n");

    printf("'''''''''\n");

}



extern int errno;

void main ()

{

 int clientCnt = 5;

 char c;

 int clientFd;

 FILE * fp;

 char hostname[64]; struct hostent * hp;

 struct sockaddr_in sin;

 /* в этом примере клиент и сервер выполняются на одном компьютере, но программа

 легко обобщается на случай разных компьютеров. Для этого можно, например,

 использовать хост-имя не собственного компьютера, как ниже, а имя

 компьютера, на котором выполняется процесс-сервер */

 // прежде всего получаем hostname собственной ЭВМ:

 gethostname (hostname, sizeof (hostname));

 // затем определяем сетевой номер своей машины:

 if ((hp = gethostbyname (hostname)) == NULL) {

 fprintf (stderr, "%s: unknown host.\n", hostname);

 exit (1);

}

// получаем свой сокет-дескриптор:

 if ((clientFd = socket (AF_INET, SOCK_STREAM, 0)) < 0) 

 {

    perror ("client: socket");

    exit (1);

 }



 //fcntl(clientFd, F_SETFL, O_NONBLOCK); 



 // создаем адрес, по которому будем связываться с сервером:

 sin.sin_family = AF_INET; sin.sin_port = htons (1234);

 // копируем сетевой номер:

 memcpy(&sin.sin_addr, hp -> h_addr, hp->h_length);

 // пытаемся связаться с сервером:

 if ( connect ( clientFd, (struct sockaddr *)&sin, sizeof (sin)) < 0 ) { perror ("client: connect"); exit (1);

 }





 //сначала отправляем серверу своё имя:

 char clientName[30];

 printf("eneter your name: ");

 fgets(clientName, sizeof(clientName), stdin);

 clientName[strlen(clientName)-1] = '\0';

 //printf("You entered name: %s\n", clientName);

 //printf("\n");

 send(clientFd, clientName, strlen(clientName), 0); 



 //создаем лист с именами приватных собеседников:

 Node *privList = NULL;



 //потом пишем серверу

 while(1 == 1)

 {    

    fd_set readset;

    FD_ZERO(&readset);

    FD_SET(clientFd, &readset);

    FD_SET(SOCKET_STDIN, &readset);

    //stdin = 0

    //clientFd - max

    //printf("Before select: \n");    

    select(clientFd+1, &readset, NULL, NULL, NULL);

    //printf("After select: \n"); 

    if(FD_ISSET(SOCKET_STDIN, &readset))

    {

        char message[256];

        memset(message, 0, sizeof(message));

        fgets(message, sizeof(message), stdin);

        message[strlen(message)-1] = '\0';

    

        if(message[0] == 'Q' && message[1] == 'u' && 

           message[2] == 'i' && message[3] == 't' && message[4] == '\0')

        {   

            //printf("%s leaves server\n", clientName);

            send (clientFd, message, strlen(message), 0);

            char byeMessage[256];

            memset(byeMessage, 0, sizeof(byeMessage));

            printf("enter your good-bye message: ");

            fgets(byeMessage, sizeof(byeMessage), stdin);

            byeMessage[strlen(byeMessage)-1] = '\0';

            //printf("\n");

            send(clientFd, byeMessage, strlen(byeMessage), 0);

            printf("you left server\n");

            break;

        }

        else if(message[0] == 'H' && message[1] == 'e' &&

                message[2] == 'l' && message[3] == 'p' && message[4] == '\0')

        {

            printCommands();

            continue;

        }

        else if(message[0] == 'U' && message[1] == 's' && message[2] == 'e' &&

                message[3] == 'r' && message[4] == 's' && message[5] == '\0')

        {

            //printf("Sending msg: Users\n");

            send(clientFd, message, strlen(message), 0);

            //printf("msg sent\n");

        }

        else if(message[0] == 'P' && message[1] == 'r' && message[2] == 'i' &&

                message[3] == 'v' && message[4] == 'a' && message[5] == 't' &&

                message[6] == 'e' && message[7] == '\0')

        {

            send(clientFd, message, strlen(message), 0);

            char Name[30];

            char privMsg[256];

            memset(Name, 0, sizeof(Name));

            memset(privMsg, 0, sizeof(privMsg));



            printf("Input private name: ");

            fgets(Name, sizeof(Name), stdin);

            Name[strlen(Name)-1] = '\0';

            pushBack(&privList, 0, Name);

            send(clientFd, Name, strlen(Name), 0); //отправили имя

                    

            printf("Input private message: ");

            fgets(privMsg, sizeof(privMsg), stdin);

            privMsg[strlen(privMsg)-1] = '\0';

            

            //printf("Sending priv Name: %s, Msg: %s\n", Name, privMsg);

            send(clientFd, privMsg, strlen(privMsg), 0); //отправили сообщение

            //printf("Sent priv name & msg\n");

        

        }

        else if(message[0] == 'P' && message[1] == 'r' && message[2] == 'i' &&

                message[3] == 'v' && message[4] == 'a' && message[5] == 't' &&

                message[6] == 'e' && message[7] == 's' && message[8] == '\0')

        {

            printPrivates(privList);

        }

        else

        {

            send(clientFd, message, strlen(message), 0);

            memset(message, 0, strlen(message));

        }

    }

    else if(FD_ISSET(clientFd, &readset))

    {

        char message1[256] = "";

        memset(message1, 0, sizeof(message1));

        //printf("client receiving msg:\n");

        recv(clientFd, message1, sizeof(message1), 0);

        //printf("client received msg: %s\n", message1);



        if(strcmp(message1, "are you online?") == 0)

            continue;

        else

        {

            printf("%s", message1);

        }

        memset(message1, 0, sizeof(message1));

        /*else if(strcmp(message, "usrOnline"))

        {

            int cntOnline;

            printf("receiving online:\n");

            recv(clientFd, &cntOnline, sizeof(cntOnline), 0);

            printf("received online: %d\n", cntOnline);

            if(cntOnline == 1)

                printf("No one but you is online\n");

            else if(cntOnline > 1)

            {

                char onlineUsers[256] = "";

                recv(clientFd, onlineUsers, sizeof(onlineUsers), 0);

                printf("'''''''''\n");

                printf("Online users:\n");

                printf("%s", onlineUsers);

                printf("\n'''''''''\n");

                memset(onlineUsers, 0, sizeof(onlineUsers));

            }

        }

        else if(strcmp(message, "msg"));

        {   

            char Name[30] = "";

            char userMsg[256] = "";

            recv(clientFd, Name, sizeof(Name), 0);

            recv(clientFd, userMsg, sizeof(userMsg), 0);

            printf("%s: %s\n", Name, userMsg);

        }*/

     }

 }



 close (clientFd);

 exit (0);

}

