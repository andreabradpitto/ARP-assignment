//This header stores all the definitions that the nodes need
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
#define SIZE 10

#define h_addr h_addr_list[0] /* for backward compatibility */

//set message connection speed (baud rate?) (forse no, lo si fa con comunicazione client server tipo listen) (no però qui va settata...)

int run_mode = 0; //set to 0 to go debug mode, 1 for multiple machine

float rf = 1; //sine wave frequency

useconds_t waiting_time = 1000; //waiting time (in microseconds) applied by process P before sending the updated token

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

#endif

//Servono 2 terminali attivi (per lo meno nella debug mode): uno per il main, e uno per lanciare i signals ad S!
//vedi da fra bruno e ariel come hanno fatto per avere più terminali...

//Qui devo mettere i parametri pre-run per i 4 precessi, e va modificato prima di essere lanciato a seconda del caso.
//Deve includere IP del mio pc e di quelli adiacenti, frequenza di riferimento del token, altri dati.