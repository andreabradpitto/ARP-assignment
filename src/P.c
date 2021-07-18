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
#include <sys/prctl.h> // required by prctl()
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

    token_strc token;
    token.token_value = 0;
    token.token_timestamp = time(NULL); // get the current time and store it in token_timestamp
    struct message msg;

    struct timeval tv;
    int retval = 0; // variable used to store select() ouput

    float dt = 0;   // time delay between reception and delivery time instants of the token
    clock_t t = 0;

    int n;          // write() handle
	int first_token = 1; // used to acknowledge the very first token received

    int sockfd;     // socket file descriptor
    int portno;     // stores the port number on which the server accepts connections
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // create a new socket
    if (sockfd < 0)
        error("\nError creating a new socket (P process)");
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        error("\nP: setsockopt(SO_REUSEADDR) failed");

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
    n = write(sockfd, &token, sizeof(token_strc));
    if (n < 0)
        error("\nError writing to socket");

    tv.tv_sec = 2;  // amount of seconds the select listens for incoming data from either pipe 1 and 2
    tv.tv_usec = 0; // same as the previous line, but with microseconds
    fd_set readfds; // set of involved pipes from which P needs to read through the select
    FD_ZERO(&readfds);               // inizialization of the set
    FD_SET(atoi(argv[0]), &readfds); // addition of the desired pipe ends to the set (read from S)
    FD_SET(atoi(argv[2]), &readfds); // addition of the desired pipe ends to the set (read from G)
    int maxfd = atoi(argv[0]) > atoi(argv[2]) ? atoi(argv[0]) : atoi(argv[2]);

    while (1)
    {
        if (state == 1) // token computation is active
        {
            retval = select(maxfd + 1, &readfds, NULL, NULL, &tv);

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
                        msg.timestamp = time(NULL);
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "pause" command acknowledgement to L
                        break;
                    case 1: // continue token computation: state is unchanged
                        msg.status = state;
                        msg.value = 0;
                        msg.timestamp = time(NULL);
                        printf("\nNodes are already running, no need to unpause");
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "continue" command acknowledgement to L
                        break;
                    case 3: // send print command to L
                        msg.status = state;
                        msg.value = 0;
                        msg.timestamp = time(NULL);
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "print" command acknowledgement to L
                        break;
                    }
                }
                if (FD_ISSET(atoi(argv[2]), &readfds)) // read of second pipe (data incoming from G) is ready
                {
                    /* Attenzione al caso RUN_MODE = 1, qui il tizio prima deve mandarmi dati sulla pipe 2 coerentemente
                    con come sto facendo io. Mi basta il token (float) da lui */

                    read(atoi(argv[2]), &token, sizeof(token_strc));
                    msg.status = 99; // special code to distinguish data coming from the 2nd pipe (G -> P)
                    msg.value = token.token_value;
                    msg.timestamp = token.token_timestamp;

                    if (first_token) // the token value depends on time delays, so skip the first one received, as it is always 0
                    {
                        first_token = 0;
                    }
                    else
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "data reception" acknowledgement to L

                    // This section is related to the communication with G, as the one with L is completed
                    t = clock() - t;
                    dt = ((float)t) / ((float)CLOCKS_PER_SEC); // by doing like this, the very first cycle has a meaningless dt value (i.e. 0)
                    token.token_value = msg.value + dt * (1 - powf(msg.value, 2) / 2) * 2 * M_PI * RF;
                    t = clock();
                    //printf("[P node sending message]\n");
                    usleep(WAITING_TIME_MICROSECS); // waiting time, in microseconds, applied to process P before it can send the updated token
                    token.token_timestamp = time(NULL);
                    n = write(sockfd, &token, sizeof(token_strc));
                    if (n < 0)
                        error("\nError writing to socket");
                }
            }
            else if (retval == 0)
                printf("\nNo data has been written into pipes in the last 2 seconds");
        }

        else // state = 0: token computation is paused
        {
            retval = select(maxfd + 1, &readfds, NULL, NULL, &tv);

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
                        msg.timestamp = time(NULL);
                        printf("\nProcesses are already stopped, no need to pause");
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "pause" command acknowledgement to L
                        break;
                    case 1: // resume token computation
                        state = 1;
                        msg.status = state;
                        msg.value = 0;
                        msg.timestamp = time(NULL);
                        write(atoi(argv[5]), &msg, sizeof(struct message)); // send "continue" command acknowledgement to L
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
