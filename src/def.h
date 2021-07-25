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

#define LOCAL_IP "localhost" // localhost name (equivalent to 127.0.0.1 and to machine's own name, i.e. Linux's "Hostname")
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
	float value;			  // if status == 8 | satus == 9 this stores token.value, irrelevant otherwise
};

typedef struct token_struct
{
	struct timeval timestamp; // timestamp of the token
	float value;			  // actual token value
} token;					  // struct alias

struct configuration
{
	int run_mode;				// set to 0 to go debug mode (= covid/V2.0 mode), 1 for multiple machine mode [default: 0]
	double rf;					// sine wave frequency [default: 1.0]
	int waiting_time_microsecs; // waiting time, in microseconds, applied by process P before sending the updated token [default: 1000]
	char *next_ip;				// IP address of the next machine in the chain ("hostname -I" to get your current IP) [default: 192.168.1.106]
	int next_port;				// chosen remote port for the communication [default: 5000]
	char *fifo1;				// name of the first fifo (i.e. named pipe) [default: npipe1]
	char *fifo2;				// name of the second fifo (i.e. named pipe) [default: npipe2]
};

void error(const char *msg) // display a message about the error on stderr and then abort the program
{
	perror(msg);
	exit(0);
}

#endif
