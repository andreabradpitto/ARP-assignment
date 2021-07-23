// 2019 ARP assignment V2.0
// Andrea Pitto - S3942710
// 10 - 07 - 2020

// This process can be run in 2 modes: debug mode (i.e. single machine/covid/V2.0 - run_mode = 0) or
// normal mode (i.e.communicating with other PCs - run_mode = 1). In the first case it receives tokens
// from P, and then sends them back to it. In the other scenario, it sends data to the P of the next PC
// in the chain

#include "def.h"

struct configuration configLoader(char *, struct configuration); // configuration loader function declaration

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

	token token;
	token.value = 0;
	gettimeofday(&token.timestamp, NULL); // get the current time and store it in timestamp

    struct configuration config;
    char *configpath = "config"; // specify config file path
    config = configLoader(configpath, config);

	int sockfd; // socket file descriptor
	int newsockfd;
	int portno; // port of the server for the client connection
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n; // read() handle
	char *fancy_time;

	if (!config.run_mode)
	{
		portno = LOCAL_PORT;
	}
	else
	{
		portno = config.next_port;
	}

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

			fancy_time = ctime(&token.timestamp.tv_sec);
			fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
			printf("\nG: Token timestamp (fancy): %s | Token value: %9f", fancy_time, token.value);
			write(atoi(argv[3]), &token, sizeof(token));
		}
	}

	close(atoi(argv[3]));
	close(sockfd);
	return 0;
}

// Load the values inside the config file and store them into constants
struct configuration configLoader(char *path, struct configuration conf)
{
    FILE *config_file = fopen(path, "r"); // open the config file in read mode
    int line_out;
    char *line = NULL;
    size_t len;

    if (config_file == NULL)
    {
        perror("Could not open config file");
    }

    // Read 1st line of the config file (run_mode)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf.run_mode = atoi(line);
    }
    else
        perror("Error reading 1st line of config file");

    // Read 2nd line of the config file (rf)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf.rf = atof(line);
    }
    else
        perror("Error reading 2nd line of config file");

    // Read 3rd line of the config file (waiting_time_microsecs)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf.waiting_time_microsecs = atoi(line);
    }
    else
        perror("Error reading 3rd line of config file");

    // Read 4th line of the config file (next_ip)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf.next_ip = line;
    }
    else
        perror("Error reading 4th line of config file");

    // Read 5th line of the config file (next_port)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf.next_port = atoi(line);
    }
    else
        perror("Error reading 5th line of config file");

    // Read 6th line of the config file (fifo1)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf.fifo1 = line;
    }
    else
        perror("Error reading 6th line of config file");

    // Read 7th line of the config file (fifo2)
    if ((line_out = getline(&line, &len, config_file)) != -1)
    {
        conf.fifo2 = line;
    }
    else
        perror("Error reading 7th line of config file");

    // Close the config file
    fclose(config_file);

    return conf;
}
