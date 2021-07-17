//This header stores all the definitions that the processes need

#ifndef CONFIG_H
#define CONFIG_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NEXT_IP '192.168.1.233' //IP address of the next machine in the chain
#define NEXT_PORT 5000			//chosen port for the communication

//attenzione al caso run_mode = 1, quando dò G a un altro tizio, assicurati che sia pronto a ricevere da atoi(argv[3]),
//quindi su (argv[2]). Vale il contrario per il caso di P

#define buff 250000
#define SIZE 2

#define h_addr h_addr_list[0] /* for backwards compatibility */

//set message connection speed (baud rate?) (forse no, lo si fa con comunicazione client server tipo listen) (no però qui va settata...)

int run_mode = 0; //set to 0 to go debug mode, 1 for multiple machine

float rf = 100; //sine wave frequency

//I did not put the waiting time here (but in the P process) because, for some reason, it did not work

struct message
{
	time_t timestamp;
	float value; //prima era di tipo char
	int status;
};
struct message msg[buff]; //definition of the message		//forse da togliere

typedef struct
{
	float token_value;
	time_t token_timestamp;
} token_struct;

char *pretty_time;

#endif
