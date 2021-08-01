// 2019 ARP assignment V2.0
// Andrea Pitto - S3942710
// 10 - 07 - 2020

// This header stores all the definitions and settings needed by the processes

#ifndef DEF_H
#define DEF_H

// Inclusion of POSIX libraries
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>

#define LOCAL_IP "localhost" // localhost name (equivalent to 127.0.0.1 and to machine's own name, i.e. Linux's "hostname")
#define LOCAL_PORT 5000		 // chosen local port for the communication

#ifndef M_PI
#define M_PI 3.14159265358979323846 // pi value definition
#endif

#define MAX_REQS 5 // maximum number of allowed requests on sockets

struct log_message
{
	struct timeval timestamp; // timestamp of the message
	int status;				  // status = 0: pause; status = 1: computing; status = 3: open log file;
							  // status = 8 new token data value from G; status = 9 new token data value from P
	float value;			  // if status == 8 || satus == 9 this stores token.value, irrelevant otherwise
};

typedef struct token_struct
{
	struct timeval timestamp; // timestamp of the token
	float value;			  // actual token value
} token;					  // struct alias

struct configuration
{
	int run_mode;				// set to 0 to go in Debug mode (= Single-machine mode), to 1 for Multi-machine mode [default: 0]
    int chain_starter;          // in Multi-machine mode, set to 1 to flag this machine as the one starting the P-G communication.
                                // Only a single PC in the chain should have this set to 1 [default: 0]
 	double rf;					// sine wave frequency [default: 1.0]
	int waiting_time_microsecs; // waiting time, in microseconds, applied by process P before sending the updated token [default: 1000]
	char *next_ip;				// IP address of the next machine in the chain ("hostname -I" to get your current IP) [default: 192.168.1.12]
	int next_port;				// chosen remote port for the communication [default: 5000]
	char *fifo;					// name of the fifo (i.e. named pipe) [default: npipe]
};

void error(const char *msg) // display a message about the error on stderr and then abort the program
{
	perror(msg);
	exit(0);
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

    // Read 2nd line of the config file (chain_starter)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf->chain_starter = atoi(line);
    }
    else
        perror("Error reading 2nd line of config file");

    // Read 3rd line of the config file (rf)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf->rf = atof(line);
    }
    else
        perror("Error reading 3rd line of config file");

    // Read 4th line of the config file (waiting_time_microsecs)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf->waiting_time_microsecs = atoi(line);
    }
    else
        perror("Error reading 4th line of config file");

    // Read 5th line of the config file (next_ip)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        if (line_out > 0 && line[line_out - 1] == '\n')
        {
            line[line_out - 1] = '\0';
        }
        conf->next_ip = strdup(line);
    }
    else
        perror("Error reading 5th line of config file");

    // Read 6th line of the config file (next_port)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf->next_port = atoi(line);
    }
    else
        perror("Error reading 6th line of config file");

    // Read 7th line of the config file (fifo)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        if (line_out > 0 && line[line_out - 1] == '\n')
        {
            line[line_out - 1] = '\0';
        }
        conf->fifo = strdup(line);
    }
    else
        perror("Error reading 7th line of config file");

    // Close the config file
    free(line);
    fclose(config_file);
}

#endif
