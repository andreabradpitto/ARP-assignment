// This header stores all the definitions needed by the processes

#ifndef CONFIG_H
#define CONFIG_H

#define NEXT_IP "192.168.1.233" 			// IP address of the next machine in the chain
#define NEXT_PORT 5000						// chosen remote port for the communication
#define RF 100.0 							// sine wave frequency [default: 100.0]
#define WAITING_TIME_MICROSECS 1000000 		// waiting time, in microseconds, applied by process P before sending the updated token [default: 1000]
#define RUN_MODE 0 							// set to 0 to go debug mode, 1 for multiple machine

#define LOCAL_IP "localhost" 				// localhost name (equivalent to 127.0.0.1 and machine's own name, Ubuntu's "Hostname")
#define LOCAL_PORT 5000						// chosen local port for the communication

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define h_addr h_addr_list[0] // added for backwards compatibility

//attenzione al caso run_mode = 1, quando do G a un altro tizio, assicurati che sia pronto a ricevere da atoi(argv[3]),
//quindi su (argv[2]). Vale il contrario per il caso di P

//set message connection speed (baud rate?) (forse no, lo si fa con comunicazione client server tipo listen) (no però qui va settata...)

struct message
{
	time_t timestamp;
	float value; //prima era di tipo char
	int status;
};

typedef struct token_struct
{
	time_t token_timestamp;
	float token_value;
} token_strc;

#endif
