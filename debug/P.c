#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <math.h>
#include "config.h"

//This node is the computational core. It is also the nevralgic waypoint of communications: all other nodes involved are
//in some way or another bond to P

/*
Instructions for compiling this node (explanation in italian):
La compilazione va a buon fine, ma il linker non trova il riferimento alla funzione pow().
Quando usi funzioni dall'header <math.h> con GCC, devi specificare esplicitamente la libreria "m" (per "math")
passando come argomento "-lm" (elle minuscola seguita dalla libreria, in questo caso "m") (-lm va messa alla fine del comando):
gcc P.c -o P -lm
gcc P.c -o P -Wall -Wextra -Werror -pedantic -lm
aggiungi anche -std=c11, dato che di default GCC usa estensioni fuori standard
*/

void error(const char *m) //Display a message about the error on stderr and then aborts the program
{
    perror(m);
    exit(0);
}

int main(int argc, char *argv[])
{
    close(atoi(argv[1]));
    close(atoi(argv[3]));
    close(atoi(argv[4]));

    int state = 1;

    token_struct token;
    token.token_value = 1;
    token.token_timestamp = time(NULL);

    struct timeval tv;
    int retval = 314; //test value for debugging purposes

    float dt = 0; //time delay between reception and delivery time instants of the token
    clock_t t = 0;

    pid_t Ppid;
    Ppid = getpid();
    printf("P: my PID is %d\n", Ppid);
    //roberto: char array with just a float inside
    //new token = received token + DT x (1. - (received token)^2/2) x 2 pi x RF

    struct message msg;
    //struct in_addr addr;
    char address[13] = "192.168.1.233"; //dovrei usare NEXT_IP ma non riesco a farlo funzionare

    int sockfd; //socket file descriptor
    int portno; //stores the port number on which the server accepts connections
    int n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    portno = 5000;                            //port number definition
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //create a new socket
    if (sockfd < 0)
    {
        error("Error creating a new socket\n");
    }

    if (!run_mode)
    {
        server = gethostbyname("ZenBook");
    }
    else
    {
        server = gethostbyname(address);
        portno = NEXT_PORT;
    }

    if (server == NULL)
    {
        fprintf(stderr, "Could not find matching host name\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr)); //the function bzero() sets all values inside a buffer to zero
    serv_addr.sin_family = AF_INET;               //this contains the code for the family of the address
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Connection failed");

    printf("[P node sending FIRST message]\n");
    n = write(sockfd, &(token), sizeof(token_struct));
    if (n < 0)
        error("Error writing to socket\n");

    while (1)
    {
        tv.tv_sec = 2;                   //amount of seconds the select listens for incoming data from either pipe 1 and 2
        tv.tv_usec = 0;                  //same as the previous line, but with microseconds
        fd_set readfds;                  //set of involved pipes from which P needs to read through the select
        FD_ZERO(&readfds);               //inizialization of the set
        FD_SET(atoi(argv[0]), &readfds); //addition of the desired pipe ends to the set
        FD_SET(atoi(argv[2]), &readfds);

        if (state == 1) //running situation
        {
            retval = select(atoi(argv[2]) + 1, &readfds, NULL, NULL, &tv);
            //https://stackoverflow.com/questions/39183953/select-always-return-1-after-create-socket-and-fd-set-in-linux
            printf("retval: %d\n", retval); //debug: il retval è 1 al primo giro poi 0 per sempre

            if (retval == -1)
            {
                perror("Select failed\n");
            }

            else if (retval > 0)
            {
                //Il mio codice arriva fino a qui, poi si blocca!
                if (FD_ISSET(atoi(argv[0]), &readfds)) //read of first pipe (data coming from S) is ready
                {
                    printf("controllo dentro al while di P");
                    switch ((atoi(argv[0])))
                    {
                    case 0:
                        state = 0;
                        msg.status = state;
                        msg.value = 0; //prima era "msg.value = '0';"
                        msg.timestamp = time(NULL);
                        write(atoi(argv[5]), &msg, sizeof(struct message)); //send "pause" command acknowledgement to L (the whole message is sent)
                        //non so se sizeof(message) sia corretto, e se sulla pipe posso mandare queste struct
                        //prima aveva usato fprintf per fare msg.timestamp e non ho capito come mai... probabilmente errore enorme
                        break;
                    case 1:
                        msg.status = state; //state is equal to 1 here
                        msg.value = 0;
                        msg.timestamp = time(NULL);
                        printf("Nodes are already running, no need to unpause\n");
                        write(atoi(argv[5]), &msg, sizeof(struct message)); //send "continue" command acknowledgement to L (the whole message is sent)
                        break;
                    case 3:
                        //send print command into the third pipe
                        msg.status = state; // state is equal to 3 here
                        msg.value = 0;
                        msg.timestamp = time(NULL);
                        write(atoi(argv[5]), &msg, sizeof(struct message)); //send "print" command acknowledgement to L (the whole message is sent)
                        break;
                    }
                }
                if (FD_ISSET(atoi(argv[2]), &readfds)) //read of second pipe (data coming from G) is ready
                {
                    /*Attenzione al caso run_mode = 1, qui il tizio prima deve mandarmi dati sulla pipe 2 coerentemente
                    con come sto facendo io. Mi basta il token (float) da lui*/

                    read(atoi(argv[2]), &(token), sizeof(token_struct)); //qui anche posso fare per indirizzo (&), passo tutto msg
                    msg.status = 99;                                     // special code to distinguish data coming from 2nd pipe
                    msg.value = token.token_value;                       //ma come lo sa che è un float?! (prima c'era " = buffer")
                    //qui infatti è da sistemare: devo anche ricevere il timestamp, e quindi devo risolvere il problema di
                    //mandare delle struct in pipe
                    msg.timestamp = time(NULL);

                    write(atoi(argv[5]), &msg, sizeof(struct message)); //send "data reception" acknowledgement to L (the whole message is sent)

                    msg.timestamp = time(NULL); //get the current time

                    //This section is related to the communication with G, as the one with L is all set
                    t = clock() - t;
                    dt = ((float)t) / ((float)CLOCKS_PER_SEC); //by doing like this, the first cycle (and only that one) has a meaningless dt value
                        token.token_value = msg.value + dt * (((float)1) - (powf(msg.value, ((float)2)) / ((float)2))) * ((float)2) * ((float)M_PI) * rf;
                    t = clock();
                    printf("[P node sending message]\n");
                    usleep(waiting_time);
                    token.token_timestamp = msg.timestamp;
                    n = write(sockfd, &token, sizeof(token_struct));
                    if (n < 0)
                        error("Error writing to socket\n");
                }
            }
            else if (retval == 0)
                printf("No data written sent to pipes in the last 2 seconds\n");
            //questo l'ho commentato perchè a runtime mi inonda il terminale
        }

        else if (state == 0) //pausing sitation
        {
            retval = select(atoi(argv[2]) + 1, &readfds, NULL, NULL, &tv);

            if (retval == -1)
            {
                perror("Select failed\n");
            }

            else if (retval > 0)
            {
                if (FD_ISSET(atoi(argv[0]), &readfds)) //read of first pipe (data coming from S) is ready
                {
                    switch ((atoi(argv[0])))
                    {
                    case 0:
                        msg.status = state; //state is equal to 0 here
                        msg.value = 0;
                        msg.timestamp = time(NULL);
                        printf("Nodes are already stopped, no need to pause\n");
                        write(atoi(argv[5]), &msg, sizeof(struct message)); //send "pause" command acknowledgement to L (the whole message is sent)
                        break;
                    case 1:
                        state = 1;
                        msg.status = state; //state is equal to 1 here
                        msg.value = 0;
                        msg.timestamp = time(NULL);
                        write(atoi(argv[5]), &msg, sizeof(struct message)); //send "continue" command acknowledgement to L (the whole message is sent)
                        break;
                    }
                }
            }
        }
    }
    close(atoi(argv[0]));
    close(atoi(argv[2]));
    close(atoi(argv[5]));
    close(sockfd);
    return 0;
}