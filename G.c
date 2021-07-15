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
#include "config.h"

//This process can be run in 2 modes: debug mode (single machine) or normal mode (communicating with other PCs);
//in the first case it receives tokens from P and then sends them back to it, in the other scenario it still
//receives data from P, but the token is sent to another machine

void error(const char *m) //display a message about the error on stderr and then abort the program
{
	perror(m);
	exit(1);
}

int main(int argc, char *argv[])
{
	close(atoi(argv[0]));
	close(atoi(argv[1]));
	close(atoi(argv[2]));
	close(atoi(argv[4]));
	close(atoi(argv[5]));

	int sockfd; //socket file descriptor
	int newsockfd;
	int portno; //port of the server for the client connection
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	token_struct token;
	token.token_value = 5;
	token.token_timestamp = time(NULL);

	//il buffer lo prendo dal main...
	//char buffer[256]; //qua è un casino coi dati capire se mi serva o no; credo di sì, proviene da esempio server.c "del prof"

	pid_t Gpid;
	Gpid = getpid();
	printf("G: my PID is %d\n", Gpid);

	sockfd = socket(AF_INET, SOCK_STREAM, 0); //create a new socket
	if (sockfd < 0)
		error("\nError creating a new socket");
	bzero((char *)&serv_addr, sizeof(serv_addr)); //the function bzero() sets all values inside a buffer to zero
	portno = 5000;
	serv_addr.sin_family = AF_INET; //this contains the code for the family of the address
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) //the bind() system call binds a socket to an address
		error("\nError on binding");
	listen(sockfd, 5); //system call that allows the process to listen for connections over the socket
	//printf("[G node waiting for messages]\n");
	if (!run_mode)
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
		//The accept() system call causes the process to block until a client connects to the server
		if (newsockfd < 0)
		{
			perror("\n'accept()' system call failed");
			return 1;
		}
		else
		{
			//puts("Connection accepted\n");
			sleep(1);

			while (1)
			{
				n = read(newsockfd, &token, sizeof(token_struct));
				if (n < 0)
					error("\nError reading from socket");

				//printf("\nHere is the message: %f | received at: %li", token.token_value, token.token_timestamp);
				pretty_time = ctime(&token.token_timestamp);
				printf("\nHere is the message: %f | received at: %s", token.token_value, pretty_time);
				write(atoi(argv[3]), &token, sizeof(token_struct));
			}
		}
	}
	else //This is the code relative to the multiple machine case
	{
		//close(atoi(argv[3])); /*This has to be discussed with the guy whose machine is the next of the chain*/
		while (1)
		{
			portno = NEXT_PORT; //in realtà questo credo sia inutile, se non lo metto rimane 5000 come ho messo io,
								//tanto il client che si connette è il mio P, del quale scelgo io la porta. Diverso il caso del nome della macchina

			//cioé così non mi cambia nulla tra le due run_mode (portno già è inutile)... forse è ok?
			printf("\nHere is the message: %f | received at: %li", token.token_value, token.token_timestamp);
			write(atoi(argv[3]), &token, sizeof(token_struct));
		}
	}

	close(sockfd);
	return 0;
}