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
#include <sys/time.h> //questo è il primo degli aggiunti
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <math.h>
#include "config.h"

// This is the main, you have to only execute this command: ./main (you can also run sudo netstat -tulpn for socket troubleshooting)
// The duty of this piece of code is to load config data and to launch all the needed processes (S, P, G and L)

// potrei aggiungere degli sleep(1) all'inzio di ogni processo per aspettare siano tutti pronti...

int main(int argc, char *argv[]) //int main(int argc, char *argv[])
{
	//pid_t Ppid, Gpid, Spid, Lpid;
	int P, G, S, L;

	int pfd1[2]; //file descriptors for pipe 1
	int pfd2[2]; //file descriptors for pipe 2
	int pfd3[2]; //file descriptors for pipe 3

	//const int BSIZE = 100;
	//char buf[BSIZE]; // mi serve per P read da G

	//float token = 1;
	//int state = 1; // state = 0 is the pause flag, = 1 equals unpause //FORSE da shiftare tutto? non credo, ma potrebbe
	//intereferire con gli std out e in eccetera
	//int logprint = 2; // when = 3, issue a log print (2 is default value: it does nothing) direi che non serve più

	int wait_status = 0;

	if (pipe(pfd1) < 0) //error condition on pipe 1
	{
		perror("Pipe 1 creation error");
		return -1;
	}
	if (pipe(pfd2) < 0) //error condition on pipe 2
	{
		perror("Pipe 2 creation error");
		return -1;
	}
	if (pipe(pfd3) < 0) //error condition on pipe 2
	{
		perror("Pipe 3 creation error");
		return -1;
	}

	char read1[SIZE];
	char write1[SIZE];
	char read2[SIZE];
	char write2[SIZE];
	char read3[SIZE];
	char write3[SIZE];

	sprintf(read1, "%d", pfd1[0]);	//load the fd input/output (3rd arg.) into the char array (1st arg.),
	sprintf(write1, "%d", pfd1[1]); //while formatting it as stated in 2nd arg.
	sprintf(read2, "%d", pfd2[0]);
	sprintf(write2, "%d", pfd2[1]);
	sprintf(read3, "%d", pfd3[0]);
	sprintf(write3, "%d", pfd3[1]);

	argv[0] = read1;  //pipe1: read
	argv[1] = write1; //pipe1: write
	argv[2] = read2;  //pipe2: read
	argv[3] = write2; //pipe2: write
	argv[4] = read3;  //pipe3: read
	argv[5] = write3; //pipe3: write
					  //I will be passing to each node all the pipe ends, by transforming their fd in char and then reverting them to integers

	G = fork();

	if (G < 0) //error condition on fork
	{
		perror("Fork G");
		return -1;
	}

	if (G == 0) //G process
	{
		char *node_name = "./G";
		if (execvp(node_name, argv) < 0) //error handling for file G
		{
			perror("Exec failed for G");
			return -1;
		}
	}
	else if (G > 0)
	{
		P = fork();

		if (P < 0) //error condition on fork
		{
			perror("Fork P");
			return -1;
		}

		if (P == 0) //P process
		{
			char *node_name = "./P";
			if (execvp(node_name, argv) < 0) //error handling for file P
			{
				perror("Exec failed for P");
				return -1;
			}
		}

		printf("[all processes setup and running]\n");

		wait(&wait_status); //questo forse mi serve per far fare la scanf al figlio S
		close(pfd1[0]);
		close(pfd1[1]);
		close(pfd2[0]);
		close(pfd2[1]);
		close(pfd3[0]);
		close(pfd3[1]);
		return 0;
	}
}