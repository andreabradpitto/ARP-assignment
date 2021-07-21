// 2019 ARP assignment V2.0
// Andrea Pitto - S3942710
// 10 - 07 - 2020

// This header stores all the definitions and settings needed by the processes

#ifndef CONFIG_H
#define CONFIG_H

#define RF 1						// sine wave frequency [default: 1.0]
#define WAITING_TIME_MICROSECS 1000 // waiting time, in microseconds, applied by process P before sending the updated token [default: 1000]
#define RUN_MODE 0					// set to 0 to go debug mode (= covid/V2.0 mode), 1 for multiple machine mode [default: 0] \
									// RUN_MODE 1 may require to adjust pipe ends in main.c
#define NEXT_IP "192.168.1.106"		// IP address of the next machine in the chain ("hostname -I" to get your IP)
#define NEXT_PORT 5000				// chosen remote port for the communication
#define LOCAL_IP "localhost"		// localhost name (equivalent to 127.0.0.1 and to machine's own name, i.e. Linux's "Hostname")
#define LOCAL_PORT 5000				// chosen local port for the communication

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

#endif
