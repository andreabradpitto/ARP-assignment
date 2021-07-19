#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/prctl.h> // non-posix?
#include "config.h"

// This process can be run in 2 modes: debug mode (single machine) or normal mode (communicating with other PCs);
// in the first case it receives tokens from P and then sends them back to it, in the other scenario it still
// receives data from P, but the token is sent to another machine


void error(const char *m) // display a message about the error on stderr and then abort the program
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

	pid_t Gpid;
	Gpid = getpid();
	prctl(PR_SET_PDEATHSIG, SIGHUP); // asks the kernel to deliver the SIGHUP signal when parent dies, i.e. also terminates G
	printf("G: my PID is %d\n", Gpid);

	int sockfd; // socket file descriptor
	int newsockfd;
	int portno = LOCAL_PORT; // port of the server for the client connection // forse da eliminare del tutto
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n; // read() handle
	char *pretty_time;
	int first_token = 1; // used to acknowledge the very first token received

	token token;
	token.token_value = 0;
	token.token_timestamp = time(NULL);

	sockfd = socket(AF_INET, SOCK_STREAM, 0); // create a new socket
	if (sockfd < 0)
		error("\nError creating a new socket (G process)");
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        error("\nG: setsockopt(SO_REUSEADDR) failed");

	bzero((char *) &serv_addr, sizeof(serv_addr));  // the function bzero() sets all values inside a buffer to zero
	serv_addr.sin_family = AF_INET; 				// this contains the code for the family of the address
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //the bind() system call binds a socket to an address
		error("\nError on binding");

	listen(sockfd, MAX_REQS); // system call that allows this process to listen for connections over the socket
	//printf("[G node waiting for messages]\n");

	if (!RUN_MODE)
	{
		clilen = sizeof(cli_addr);
		// The accept() system call causes the process to block until a client connects to the server
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
		if (newsockfd < 0)
		{
			perror("\n'accept()' system call failed");
			return -1;
		}
		else // connection accepted
		{
			while (1)
			{
				n = read(newsockfd, &token, sizeof(token));
				if (n < 0)
					error("\nError reading from socket");

				if (first_token) // the token value depends on time delays, so skip the first one received, as it is always 0
				{
					first_token = 0;
					write(atoi(argv[3]), &token, sizeof(token));
				}
				else
				{
					//printf("\nG: Token timestamp: %li | Token value: %f", token.token_timestamp, token.token_value);
					pretty_time = ctime(&token.token_timestamp);
					pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
					printf("\nG: Token timestamp (fancy): %s | Token value: %f", pretty_time, token.token_value);
					write(atoi(argv[3]), &token, sizeof(token));
				}
			}
		}
	}
	else // this is the code portion relative to the multiple machine case // non penso che io debba implementarlo!
	{
		//close(atoi(argv[3])); // tyhis has to be discussed with the guy whose machine is the next of the chain
		while (1)
		{
			portno = NEXT_PORT; //in realtà questo credo sia inutile, se non lo metto rimane 5000 come ho messo io,
								//tanto il client che si connette è il mio P, del quale scelgo io la porta. Diverso il caso del nome della macchina

			//cioe cosi non mi cambia nulla tra le due RUN_MODE (portno gia e inutile)... forse e ok?
			printf("\nG: Token timestamp: %li | Token value: %f", token.token_timestamp, token.token_value);
			write(atoi(argv[3]), &token, sizeof(token));
		}
	}

	close(atoi(argv[3]));
	close(sockfd);
	return 0;
}
