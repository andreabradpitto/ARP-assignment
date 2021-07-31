// 2019 ARP assignment V2.0
// Andrea Pitto - S3942710
// 10 - 07 - 2020

// This process is the computational core. It is also the nevralgic waypoint of communications:
// all other nodes involved are, in a way or another, bond to P. This process uses different constants
// based on config.run_mode. A fake delay is added to the computation when config.run_mode = 0.
//config.run_mode = 1 scenario has P expecting data from the G process of the previous PC in the chain

#include "def.h"

void configLoader(char *, struct configuration *); // configuration loader function declaration

int main(int argc, char *argv[])
{
    close(atoi(argv[1]));
    close(atoi(argv[3]));
    close(atoi(argv[4]));

    pid_t Ppid;
    Ppid = getpid();
    printf("P: my PID is %d\n", Ppid);

    int state = 1; // state = 0: paused; state = 1: computing; state = 3: log opened (i.e. paused)

    struct log_message log_msg;

    struct timeval select_tv; // define patience (timeout) for select()
    int retval = 0;           // variable used to store the output of select()

    float dt = 0; // time delay between reception and delivery time instants of the token

    int n; // write() handle

    char *fancy_time;

    struct configuration config;
    struct configuration *configPtr = &config;
    char *configpath = "config"; // specify config file path
    configLoader(configpath, configPtr);

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

    if (!config.run_mode) // config.run_mode = 0
    {
        token token;
        token.value = 0;
        gettimeofday(&token.timestamp, NULL); // get the current time and store it in timestamp

        server = gethostbyname(LOCAL_IP);
        portno = LOCAL_PORT;

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

        // P process sending the first message, and thus starting the communication between G and itself
        gettimeofday(&token.timestamp, NULL);     // store token sending time
        n = write(sockfd, &token, sizeof(token)); // sending the new token to G
        if (n < 0)
            error("\nError writing to socket");
        log_msg.status = 9; // special code to distinguish log entries relative to tokens sent by P
        log_msg.value = token.value;
        log_msg.timestamp = token.timestamp;                        // log token sending time
        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "token sent" acknowledgment to L

        fancy_time = ctime(&token.timestamp.tv_sec);
        fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
        printf("\nP: Token timestamp (fancy): %s | Token value: %9f\r", fancy_time, token.value);
        fflush(stdout);

        // Waiting time, in microseconds, applied to process P before it can check for new incoming tokens
        usleep(config.waiting_time_microsecs);

        // Set of involved pipe ends from which P needs to read through the select
        fd_set readfds;
        int maxfd = atoi(argv[0]) > atoi(argv[2]) ? atoi(argv[0]) : atoi(argv[2]); // compute highest fd for the 1st arg. of select()

        while (1)
        {
            select_tv.tv_sec = 2;  // amount of seconds the select listens for incoming data from either pipe 1 and 2
            select_tv.tv_usec = 0; // same as the previous line, but with microseconds

            FD_ZERO(&readfds);               // initialization of the set
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
                    if (FD_ISSET(atoi(argv[2]), &readfds)) // read of second pipe (data incoming from G) is ready
                    {
                        read(atoi(argv[2]), &token, sizeof(token));

                        // Waiting time, in microseconds, applied to process P before it can check for new incoming tokens
                        usleep(config.waiting_time_microsecs);

                        gettimeofday(&log_msg.timestamp, NULL); // log token reception time
                        log_msg.status = 8;                     // special code to distinguish data coming from the 2nd pipe (G -> P)
                        log_msg.value = token.value;
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "data reception" acknowledgment to L

                        // Time delay computation
                        // It is the difference (in seconds) between the token reception time and
                        // the time present inside of the received token, which is the time at which that token had been
                        // sent by P (or the previous P in the chain)
                        dt = (log_msg.timestamp.tv_sec - token.timestamp.tv_sec) +
                             (log_msg.timestamp.tv_usec - token.timestamp.tv_usec) / (float)1000000;

                        // Token computation
                        // Using a custom formula as the one provided is not working properly
                        token.value = sin(2 * M_PI * config.rf * (log_msg.value + dt * (1 - log_msg.value))); // custom formula
                        //token.value = log_msg.value + dt * (1 - powf(log_msg.value, 2) / 2) * 2 * M_PI * config.rf; // original formula

                        gettimeofday(&token.timestamp, NULL);     // store token sending time
                        n = write(sockfd, &token, sizeof(token)); // sending the new token to G
                        if (n < 0)
                            error("\nError writing to socket");
                        log_msg.status = 9; // special code to distinguish log entries relative to tokens sent by P
                        log_msg.value = token.value;
                        log_msg.timestamp = token.timestamp;                        // log token sending time
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "token sent" acknowledgment to L

                        fancy_time = ctime(&token.timestamp.tv_sec);
                        fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
                        printf("P: Token timestamp (fancy): %s | Token value: %9f\r", fancy_time, token.value);
                        fflush(stdout);
                    }

                    if (FD_ISSET(atoi(argv[0]), &readfds)) // read of first pipe (data incoming from S) is ready
                    {
                        // read() into "state":
                        // state = 0: stop token computation;
                        // state = 1: continue token computation (state is unchanged)
                        // state = 3: request log file opening to L
                        read(atoi(argv[0]), &state, sizeof(int));
                        gettimeofday(&log_msg.timestamp, NULL);
                        log_msg.status = state;
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send pause/continue/log command acknowledgment to L
                    }
                }
            }

            else // state = 0 or 3: token computation is paused
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
                        // read() into "state":
                        // state = 0: keep computation paused (state is unchanged)
                        // state = 1: resume token computation
                        // state = 3: request log file opening to L
                        read(atoi(argv[0]), &state, sizeof(int));
                        gettimeofday(&log_msg.timestamp, NULL);
                        log_msg.status = state;
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send pause/continue/log command acknowledgment to L
                    }
                }
            }
        }

        close(atoi(argv[2]));
    }

    else // config.run_mode = 1
    {
        char token[10]; // size 9 should be also fine
        float token_value = 0;
        sprintf(token, "%f", token_value);

        struct timeval sent_ts; // timestamp for reception time

        close(atoi(argv[2]));

        mkfifo(config.fifo, 0644); // create a named pipe (grant full access to Owner, read only permission to Group and Other)
        int fifofd = open(config.fifo, O_RDONLY);

        server = gethostbyname(config.next_ip);
        portno = config.next_port;

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

        // P process sending the first message, and thus starting the communication between G and itself
        gettimeofday(&sent_ts, NULL);             // store token sending time
        n = write(sockfd, &token, sizeof(token)); // sending the new token to G
        if (n < 0)
            error("\nError writing to socket");
        log_msg.status = 9; // special code to distinguish log entries relative to tokens sent by P
        log_msg.value = token_value;
        log_msg.timestamp = sent_ts;                                // log token sending time
        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "token sent" acknowledgment to L

        fancy_time = ctime(&sent_ts.tv_sec);
        fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
        printf("\nP: Token timestamp (fancy): %s | Token value: %9f\r", fancy_time, token_value);
        fflush(stdout);

        // Set of involved pipe ends from which P needs to read through the select
        fd_set readfds;
        int maxfd = atoi(argv[0]) > fifofd ? atoi(argv[0]) : fifofd; // compute highest fd for the 1st arg. of select()

        while (1)
        {
            select_tv.tv_sec = 2;  // amount of seconds the select listens for incoming data from either pipe 1 and 2
            select_tv.tv_usec = 0; // same as the previous line, but with microseconds

            FD_ZERO(&readfds);               // initialization of the set
            FD_SET(atoi(argv[0]), &readfds); // addition of the desired pipe ends to the set (read from S)
            FD_SET(fifofd, &readfds);       // addition of the desired pipe ends to the set (read from previous G)

            if (state == 1) // token computation is active
            {
                retval = select(maxfd + 1, &readfds, NULL, NULL, &select_tv);

                if (retval == -1)
                {
                    perror("\nSelect failed");
                }

                else if (retval > 0)
                {
                    if (FD_ISSET(fifofd, &readfds)) // read of second pipe (data incoming from G) is ready
                    {
                        read(fifofd, &token, sizeof(token));

                        gettimeofday(&log_msg.timestamp, NULL); // log token reception time
                        log_msg.status = 8;                     // special code to distinguish data coming from the 2nd pipe (G -> P)
                        log_msg.value = atof(token);
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "data reception" acknowledgment to L

                        // Time delay computation
                        // It is the difference (in seconds) between the token reception time and
                        // the time present inside of the received token, which is the time at which that token had been
                        // sent by P (or the previous P in the chain)
                        dt = (log_msg.timestamp.tv_sec - sent_ts.tv_sec) +
                             (log_msg.timestamp.tv_usec - sent_ts.tv_usec) / (float)1000000;

                        // Token computation
                        // using a custom formula as the one provided is not working properly
                        token_value = sin(2 * M_PI * config.rf * (log_msg.value + dt * (1 - log_msg.value))); // custom formula
                        //token_value = log_msg.value + dt * (1 - powf(log_msg.value, 2) / 2) * 2 * M_PI * config.rf; // original formula

                        sprintf(token, "%f", token_value);
                        gettimeofday(&sent_ts, NULL);                   // store token sending time
                        n = write(sockfd, &token_value, sizeof(token)); // sending the new token to G
                        if (n < 0)
                            error("\nError writing to socket");
                        log_msg.status = 9; // special code to distinguish log entries relative to tokens sent by P
                        log_msg.value = token_value;
                        log_msg.timestamp = sent_ts;                                // log token sending time
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send "token sent" acknowledgment to L

                        fancy_time = ctime(&sent_ts.tv_sec);
                        fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
                        printf("P: Token timestamp (fancy): %s | Token value: %9f\r", fancy_time, token_value);
                        fflush(stdout);
                    }

                    if (FD_ISSET(atoi(argv[0]), &readfds)) // read of first pipe (data incoming from S) is ready
                    {
                        // read() into "state":
                        // state = 0: stop token computation;
                        // state = 1: continue token computation (state is unchanged)
                        // state = 3: request log file opening to L
                        read(atoi(argv[0]), &state, sizeof(int));
                        gettimeofday(&log_msg.timestamp, NULL);
                        log_msg.status = state;
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send pause/continue/log command acknowledgment to L
                    }
                }
            }

            else // state = 0 or 3: token computation is paused
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
                        // read() into "state":
                        // state = 0: keep computation paused (state is unchanged)
                        // state = 1: resume token computation
                        // state = 3: request log file opening to L
                        read(atoi(argv[0]), &state, sizeof(int));
                        gettimeofday(&log_msg.timestamp, NULL);
                        log_msg.status = state;
                        write(atoi(argv[5]), &log_msg, sizeof(struct log_message)); // send pause/continue/log command acknowledgment to L
                    }
                }
            }
        }

        close(fifofd);
        unlink(config.fifo);
    }

    close(atoi(argv[0]));
    close(atoi(argv[5]));
    close(sockfd);
    return 0;
}

// Load the values inside the config file and store them into constants
void configLoader(char *path, struct configuration *conf)
{
    FILE *config_file = fopen(path, "r"); // open the config file in read mode
    int line_out;
    char *line = NULL;
    size_t len = 0;

    if (config_file == NULL)
    {
        perror("Could not open config file");
    }

    // Read 1st line of the config file (run_mode)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf->run_mode = atoi(line);
    }
    else
        perror("Error reading 1st line of config file");

    // Read 2nd line of the config file (rf)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf->rf = atof(line);
    }
    else
        perror("Error reading 2nd line of config file");

    // Read 3rd line of the config file (waiting_time_microsecs)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf->waiting_time_microsecs = atoi(line);
    }
    else
        perror("Error reading 3rd line of config file");

    // Read 4th line of the config file (next_ip)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        if (line_out > 0 && line[line_out - 1] == '\n')
        {
            line[line_out - 1] = '\0';
        }
        conf->next_ip = strdup(line);
    }
    else
        perror("Error reading 4th line of config file");

    // Read 5th line of the config file (next_port)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf->next_port = atoi(line);
    }
    else
        perror("Error reading 5th line of config file");

    // Read 6th line of the config file (fifo)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        if (line_out > 0 && line[line_out - 1] == '\n')
        {
            line[line_out - 1] = '\0';
        }
        conf->fifo = strdup(line);
    }
    else
        perror("Error reading 6th line of config file");

    // Close the config file
    free(line);
    fclose(config_file);
}
