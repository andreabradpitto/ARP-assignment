#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>
#include <math.h>
#include <sys/prctl.h> // non-posix?
#include "config.h"

// This process is the computational core. It is also the nevralgic waypoint of communications:
// all other nodes involved are, in a way or another, bond to P


void error(const char *m) // display a message about the error on stderr and then abort the program
{
    perror(m);
    exit(0);
}

int main(int argc, char *argv[])
{
    close(atoi(argv[1]));
    close(atoi(argv[3]));
    close(atoi(argv[4]));

    pid_t Ppid;
    Ppid = getpid();
    prctl(PR_SET_PDEATHSIG, SIGHUP); // asks the kernel to deliver the SIGHUP signal when parent dies, i.e. also terminates P
    printf("P: my PID is %d\n", Ppid);

    int state = 1;

    token token;
    token.value = 0;
    gettimeofday(&token.timestamp, NULL); // get the current time and store it in timestamp
    struct message msg;

    struct timeval select_tv;   // defines select() patience (timeout)
    int retval = 0;             // variable used to store select() ouput

    float dt = 0;               // time delay between reception and delivery time instants of the token
    struct timeval t_received;  // time at which the token is received
    struct timeval t_sent;      // time at which the token is sent

    int n;          // write() handle

    int sockfd;     // socket file descriptor
    int portno;     // stores the port number on which the server accepts connections
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // create a new socket
    if (sockfd < 0)
    {
        error("\nError creating a new socket (P process)");
    }
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
        error("\nP: setsockopt(SO_REUSEADDR) failed");
    }

    if (!RUN_MODE)
    {
        server = gethostbyname(LOCAL_IP);
        portno = LOCAL_PORT;
    }
    else
    {
        server = gethostbyname(NEXT_IP);
        portno = NEXT_PORT;
    }

    if (server == NULL)
    {
        fprintf(stderr, "\nCould not find matching host name");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); // the function bzero() sets all values inside a buffer to zero
    serv_addr.sin_family = AF_INET;                // this contains the code for the family of the address
    bcopy((char *) server->h_addr_list[0], (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("\nConnection failed");

    //printf("[P node sending the first message]\n");
    gettimeofday(&t_sent, NULL);
    token.timestamp = t_sent;
    n = write(sockfd, &token, sizeof(token));
    if (n < 0)
        error("\nError writing to socket");

    while (1)
    {
        select_tv.tv_sec = 2;  // amount of seconds the select listens for incoming data from either pipe 1 and 2
        select_tv.tv_usec = 0; // same as the previous line, but with microseconds
        int maxfd = atoi(argv[0]) > atoi(argv[2]) ? atoi(argv[0]) : atoi(argv[2]); // compute highest fd for select() 1st arg.

        fd_set readfds;                     // set of involved pipes from which P needs to read through the select
        FD_ZERO(&readfds);                  // inizialization of the set
        FD_SET(atoi(argv[0]), &readfds);    // addition of the desired pipe ends to the set (read from S)
        FD_SET(atoi(argv[2]), &readfds);    // addition of the desired pipe ends to the set (read from G)
    
        if (state == 1) // token computation is active
        {
            retval = select(maxfd + 1, &readfds, NULL, NULL, &select_tv);

            if (retval == -1)
            {
                perror("\nSelect failed");
            }

            else if (retval > 0)
            {
                if (FD_ISSET(atoi(argv[0]), &readfds)) // read of first pipe (data incoming from S) is ready
                {
                    read(atoi(argv[0]), &state, sizeof(int));
                    switch (state)
                    {
                    case 0: // stop token computation
                        msg.status = state;
                        msg.value = 0;
                        gettimeofday(&msg.timestamp, NULL);
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "pause" command acknowledgment to L
                        break;
                    case 1: // continue token computation: state is unchanged
                        msg.status = state;
                        msg.value = 0;
                        gettimeofday(&msg.timestamp, NULL);
                        printf("\nNodes are already running, no need to unpause");
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "continue" command acknowledgment to L
                        break;
                    case 3: // send print command to L
                        msg.status = state;
                        msg.value = 0;
                        gettimeofday(&msg.timestamp, NULL);
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "print" command acknowledgment to L
                        break;
                    }
                }
                if (FD_ISSET(atoi(argv[2]), &readfds)) // read of second pipe (data incoming from G) is ready
                {
                    /* Attenzione al caso RUN_MODE = 1, qui il tizio prima deve mandarmi dati sulla pipe 2 coerentemente
                    con come sto facendo io. Mi basta il token (float) da lui */
                    read(atoi(argv[2]), &token, sizeof(token));
                    gettimeofday(&t_received, NULL);
                    msg.status = 99; // special code to distinguish data coming from the 2nd pipe (G -> P)
                    msg.value = token.value;
                    msg.timestamp = token.timestamp;
                    //printf("\nt_received.tv_sec: %li ", t_received.tv_sec);
                    //printf("t_sent.tv_sec: %li ", t_sent.tv_sec);
                    //printf("t_received.tv_usec: %li ", t_received.tv_usec);
                    //printf("t_sent.tv_usec: %li ", t_sent.tv_usec);
                    //printf("DT: %f", dt);
                    write(atoi(argv[5]), &msg, sizeof(struct message)); // send "data reception" acknowledgment to L

                    // This section is related to the communication with G, as the one with L is completed
                    dt = (t_received.tv_sec - t_sent.tv_sec) + (t_received.tv_usec - t_sent.tv_usec) / (float)1000000;
                    token.value = msg.value + dt * (1 - powf(msg.value, 2) / 2) * 2 * M_PI * RF;
                    gettimeofday(&token.timestamp, NULL);
                    n = write(sockfd, &token, sizeof(token));
                    gettimeofday(&t_sent, NULL);
                    if (n < 0)
                        error("\nError writing to socket");
                    usleep(WAITING_TIME_MICROSECS); // waiting time, in microseconds, applied to process P before it can send the updated token
                }
            }
            else if (retval == 0)
                printf("\nNo data has been written into pipes in the last 2 seconds");
        }

        else // state = 0: token computation is paused
        {
            retval = select(maxfd + 1, &readfds, NULL, NULL, &select_tv);

            if (retval == -1)
            {
                perror("\nSelect failed");
            }

            else if (retval > 0)
            {
                if (FD_ISSET(atoi(argv[0]), &readfds)) // read of first pipe (data incoming from S) is ready
                {
                    read(atoi(argv[0]), &state, sizeof(int));
                    switch (state)
                    {
                    case 0: // keep computation paused: state is unchanged
                        msg.status = state;
                        msg.value = 0;
                        gettimeofday(&msg.timestamp, NULL);
                        printf("\nProcesses are already stopped, no need to pause");
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "pause" command acknowledgment to L
                        break;
                    case 1: // resume token computation
                        state = 1;
                        msg.status = state;
                        msg.value = 0;
                        gettimeofday(&msg.timestamp, NULL);
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "continue" command acknowledgment to L
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
