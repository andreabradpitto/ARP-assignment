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

//This is the process that is used to communicate with the terminal. It receives and handles 3 different commands: start, pause, log.
//In order to use it, you have to type e.g. "kill -9 1234", where the first number is the signal chosen whilst the second
//is the PID of the node S. You only interact with S, as the rest of the code handles internally all the commands issued.

int state = 1;	  //these 2 values are stored as global variables because you cannot pass any
int logprint = 2; //argument of choice to the signal handlers

int flag1 = 0;
int flag2 = 0;
int flag3 = 0;

void start_handler(int signum)
{
	if (signum == SIGCONT)
	{
		//This handler is devoted to resuming token computation, as well as all other tasks in all the nodes
		flag1 = 1;
	}
}

void stop_handler(int signum)
{
	if (signum == SIGUSR2)
	{
		//This handler is devoted to manage pause the execution of the nodes
		flag2 = 1;
	}
}

void log_handler(int signum)
{
	if (signum == SIGUSR1)
	{
		//This handler issues the printing on terminal of the log file
		flag3 = 1;
	}
}

int main(int argc, char *argv[])
{
	close(atoi(argv[0]));
	close(atoi(argv[2]));
	close(atoi(argv[3]));
	close(atoi(argv[4]));
	close(atoi(argv[5]));

	int exit_status = system("gnome-terminal");

	pid_t Spid;
	Spid = getpid();
	printf("S: my PID is %d (-> Spid)\n", Spid);

	signal(SIGCONT, start_handler); // kill -18 Spid
	signal(SIGUSR2, stop_handler);	// kill -12 Spid
									//Con SIGSTOP mi blocca la S e non posso mandare l'avviso agli altri nodi:
									//Se faccio così (SIGUSR2) blocco tutti tranne S,
									//il quale comunque non fa nulla fino a nuovo input
	signal(SIGUSR1, log_handler);	// kill -10 Spid
	while (1)
	{
		if (flag1)
		{
			printf("\n\nReceived start command by user\n");
			state = 1;
			write(atoi(argv[1]), &state, sizeof(int)); //write(pfd1[1], &state, sizeof(int));
			flag1 = 0;
			break;
		}
		if (flag2)
		{
			printf("\n\nReceived stop command by user\n");
			state = 0;
			write(atoi(argv[1]), &state, sizeof(int)); //write(pfd1[1], &state, sizeof(int));
			flag2 = 0;
			break;
		}
		if (flag3)
		{
			printf("\n\nReceived log command by user\n");
			logprint = 3;
			write(atoi(argv[1]), &logprint, sizeof(int)); //write(pfd1[1], &logprint, sizeof(int));
			flag3 = 0;
			logprint = 2;
			break;
		}
	}

	close(atoi(argv[1]));
	return 0;
}