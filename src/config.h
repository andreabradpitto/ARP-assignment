// This header stores all the definitions needed by the processes

#ifndef CONFIG_H
#define CONFIG_H

#define NEXT_IP "192.168.1.233"		   // IP address of the next machine in the chain
#define NEXT_PORT 5000				   // chosen remote port for the communication
#define RF 100.0					   // sine wave frequency [default: 1.0]
#define WAITING_TIME_MICROSECS 1000000 // waiting time, in microseconds, applied by process P before sending the updated token [default: 1000]
#define RUN_MODE 0					   // set to 0 to go debug mode (= covid/V2.0 mode), 1 for multiple machine mode

#define LOCAL_IP "localhost" // localhost name (equivalent to 127.0.0.1 and to machine's own name, i.e. Linux's "Hostname")
#define LOCAL_PORT 5000		 // chosen local port for the communication

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_REQS 5 // maximum number of allowed requests on sockets

//attenzione al caso run_mode = 1, quando do G a un altro tizio, assicurati che sia pronto a ricevere da atoi(argv[3]),
//quindi su (argv[2]). Vale il contrario per il caso di P

struct message
{
	time_t timestamp; // timestamp of the token
	int status;		  // status = 0: pause; status = 1: computing; status = 3: log print; status = 99 token data in value
	float value;	  // if status == 99 this stores token_value, irrelevant otherwise
};

typedef struct token_struct
{
	time_t token_timestamp; // timestamp of the token
	float token_value;		// actual token value
} token;					// struct alias

#endif
