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
#include "config.h"

// This process is the computational core. It is also the nevralgic waypoint of communications:
// all other nodes involved are, in a way or another, bond to P. This process uses different constants
// based on RUN_MODE. A fake delay is added to the computation when RUN_MODE = 0. RUN_MODE = 1 scenario
// has P expecting data from the G process of the previous PC in the chain

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
    printf("P: my PID is %d\n", Ppid);

    int state = 1;

    token token;
    token.value = 0;
    gettimeofday(&token.timestamp, NULL); // get the current time and store it in timestamp
    struct log_message log_msg;

    struct timeval select_tv; // defines select() patience (timeout)
    int retval = 0;           // variable used to store select() ouput

    float dt = 0;              // time delay between reception and delivery time instants of the token
    struct timeval t_received; // time at which the token is received
    struct timeval t_sent;     // time at which the token is sent

    int n; // write() handle

    int sockfd; // socket file descriptor
    int portno; // stores the port number on which the server accepts connections
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

    bzero((char *)&serv_addr, sizeof(serv_addr)); // the function bzero() sets all values inside a buffer to zero
    serv_addr.sin_family = AF_INET;               // this contains the code for the family of the address
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("\nConnection failed");

    // P process sending the first message and starting the communication between G and itself
    gettimeofday(&t_sent, NULL);
    token.timestamp = t_sent;
    n = write(sockfd, &token, sizeof(token));
    if (n < 0)
        error("\nError writing to socket");
    gettimeofday(&t_sent, NULL);
    log_msg.status = 9; // special code to distinguish log entries relative to tokens sent by P
    log_msg.value = token.value;
    log_msg.timestamp = t_sent;
    write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "token sent" acknowledgment to L
    if (!RUN_MODE)
    {
        // Waiting time, in microseconds, applied to process P before it can check for new incoming tokens
        usleep(WAITING_TIME_MICROSECS);
    }

    // Set of involved pipe ends from which P needs to read through the select
    fd_set readfds;
    int maxfd = atoi(argv[0]) > atoi(argv[2]) ? atoi(argv[0]) : atoi(argv[2]); // compute highest fd for the 1st arg. of select()

    while (1)
    {
        select_tv.tv_sec = 2;  // amount of seconds the select listens for incoming data from either pipe 1 and 2
        select_tv.tv_usec = 0; // same as the previous line, but with microseconds

        FD_ZERO(&readfds);               // inizialization of the set
        FD_SET(atoi(argv[0]), &readfds); // addition of the desired pipe ends to the set (read from S)
        FD_SET(atoi(argv[2]), &readfds); // addition of the desired pipe ends to the set (read from G)

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
                        log_msg.status = state;
                        gettimeofday(&log_msg.timestamp, NULL);
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "pause" command acknowledgment to L
                        break;
                    case 1: // continue token computation: state is unchanged
                        log_msg.status = state;
                        gettimeofday(&log_msg.timestamp, NULL);
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "continue" command acknowledgment to L
                        break;
                    case 3: // request log file opening to L
                        log_msg.status = state;
                        gettimeofday(&log_msg.timestamp, NULL);
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "print" command acknowledgment to L
                        state = 0;                                                  // pause computation upon log file opening
                        break;
                    }
                }
                if (FD_ISSET(atoi(argv[2]), &readfds)) // read of second pipe (data incoming from G) is ready
                {
                    read(atoi(argv[2]), &token, sizeof(token));
                    gettimeofday(&t_received, NULL);
                    log_msg.status = 8; // special code to distinguish data coming from the 2nd pipe (G -> P)
                    log_msg.value = token.value;
                    log_msg.timestamp = token.timestamp;
                    write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "data reception" acknowledgment to L

                    // Time delay and token computations
                    dt = (t_received.tv_sec - t_sent.tv_sec) + (t_received.tv_usec - t_sent.tv_usec) / (float)1000000;
                    //token.value = log_msg.value + dt * (1 - powf(log_msg.value, 2) / 2) * 2 * M_PI * RF; // original formula (not working)
                    token.value = 2 * M_PI * RF * sin(log_msg.value + dt * (1 - log_msg.value)); // using this formula instead

                    gettimeofday(&token.timestamp, NULL);
                    n = write(sockfd, &token, sizeof(token)); // sending the new token to G
                    gettimeofday(&t_sent, NULL);
                    log_msg.status = 9; // special code to distinguish log entries relative to tokens sent by P
                    log_msg.value = token.value;
                    log_msg.timestamp = t_sent;
                    write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "token sent" acknowledgment to L
                    if (n < 0)
                        error("\nError writing to socket");
                    if (!RUN_MODE)
                    {
                        // Waiting time, in microseconds, applied to process P before it can check for new incoming tokens
                        usleep(WAITING_TIME_MICROSECS);
                    }
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
                        log_msg.status = state;
                        gettimeofday(&log_msg.timestamp, NULL);
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "pause" command acknowledgment to L
                        break;
                    case 1: // resume token computation
                        log_msg.status = state;
                        gettimeofday(&log_msg.timestamp, NULL);
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "continue" command acknowledgment to L
                        break;
                    case 3: // request log file opening to L
                        log_msg.status = state;
                        gettimeofday(&log_msg.timestamp, NULL);
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "print" command acknowledgment to L
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
