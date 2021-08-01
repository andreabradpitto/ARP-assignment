// 2019 ARP assignment V2.0
// Andrea Pitto - S3942710
// 10 - 07 - 2020

// This process can be run in 2 modes: Debug mode (i.e. Single-machine - config.run_mode = 0) or
// Multi-machine mode (i.e.communicating with other PCs - config.run_mode = 1).
// In the first case it receives tokens from P, and then sends them back to it.
// In the other scenario, it sends data to the P of the next PC in the chain

#include "def.h"

int main(int argc, char *argv[])
{
	close(atoi(argv[0]));
	close(atoi(argv[1]));
	close(atoi(argv[2]));
	close(atoi(argv[4]));
	close(atoi(argv[5]));

	pid_t Gpid;
	Gpid = getpid();
	printf("G: my PID is %d\n", Gpid);

	struct configuration config;
	struct configuration *configPtr = &config;
	char *configpath = "config"; // specify config file path
	configLoader(configpath, configPtr);

	int sockfd; // socket file descriptor
	int newsockfd;
	int portno; // port of the server for the client connection
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n; // read() handle

	if (!config.run_mode) // config.run_mode = 0
	{
		token token;
		token.value = 0;
		gettimeofday(&token.timestamp, NULL); // get the current time and store it in timestamp

		portno = LOCAL_PORT;

		sockfd = socket(AF_INET, SOCK_STREAM, 0); // create a new socket
		if (sockfd < 0)
			error("\nError creating a new socket (G process)");
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
			error("\nG: setsockopt(SO_REUSEADDR) failed");

		bzero((char *)&serv_addr, sizeof(serv_addr)); // the function bzero() sets all values inside a buffer to zero
		serv_addr.sin_family = AF_INET;				  // this contains the code for the family of the address
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
		if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // the bind() system call binds a socket to an address
			error("\nError on binding");

		// G process is now waiting for incoming tokens
		listen(sockfd, MAX_REQS); // system call that allows this process to listen for connections over the socket

		clilen = sizeof(cli_addr);
		// The accept() system call causes the process to block until a client connects to the server
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
		if (newsockfd < 0)
		{
			perror("\naccept() system call failed");
			return -1;
		}
		else // connection accepted
		{
			while (1)
			{
				n = read(newsockfd, &token, sizeof(token));
				if (n < 0)
					error("\nError reading from socket");

				write(atoi(argv[3]), &token, sizeof(token));
			}
		}

		close(atoi(argv[3]));
	}

	else // config.run_mode = 1
	{
		token token;
		token.value = 0;
		gettimeofday(&token.timestamp, NULL); // get the current time and store it in timestamp


		close(atoi(argv[3]));

		mkfifo(config.fifo, 0644); // create a named pipe (grant full access to Owner, read only permission to Group and Other)
		int fifofd = open(config.fifo, O_WRONLY);

		portno = config.next_port;

		sockfd = socket(AF_INET, SOCK_STREAM, 0); // create a new socket
		if (sockfd < 0)
			error("\nError creating a new socket (G process)");
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
			error("\nG: setsockopt(SO_REUSEADDR) failed");

		bzero((char *)&serv_addr, sizeof(serv_addr)); // the function bzero() sets all values inside a buffer to zero
		serv_addr.sin_family = AF_INET;				  // this contains the code for the family of the address
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
		if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // the bind() system call binds a socket to an address
			error("\nError on binding");

		// G process is now waiting for incoming tokens
		listen(sockfd, MAX_REQS); // system call that allows this process to listen for connections over the socket

		clilen = sizeof(cli_addr);
		// The accept() system call causes the process to block until a client connects to the server
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
		if (newsockfd < 0)
		{
			perror("\naccept() system call failed");
			return -1;
		}
		else // connection accepted
		{
			while (1)
			{
				n = read(newsockfd, &token, sizeof(token));
				if (n < 0)
					error("\nError reading from socket");

				write(fifofd, &token, sizeof(token));
			}
		}

		close(fifofd);
		unlink(config.fifo);
	}

	close(sockfd);
	return 0;
}
