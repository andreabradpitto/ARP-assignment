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
#include <sys/prctl.h> // required by prctl()
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

	pid_t Spid;
	Spid = getpid();
	prctl(PR_SET_PDEATHSIG, SIGHUP); // Asks the kernel to deliver the SIGHUP signal when parent dies, i.e. also terminates S

	char welcome0[1] = "";
	char welcome1[146] = "[This is the Input Terminal, through which you can send signals to the running processes. "
						 "Please also che the Output Terminal to inspect outputs]";

	//char welcome2[80] = "Available commands: 'kill -18 %d', 'kill -12 %d', 'kill -10 %d'", Spid, Spid, Spid;
	char welcome2[20] = "Available commands:";
	char command1[30] = "kill -10 ";
	char command2[30] = "kill -12 ";
	char command3[30] = "kill -18 ";
	char trail1[11] = "output log";
	char trail2[6] = "pause";
	char trail3[7] = "resume";
	char temp[20];
	sprintf(temp, "%d", Spid);
	strncat(command1, temp, (sizeof(command1) - strlen(command1)));
	strncat(command1, trail1, (sizeof(command1) - strlen(command1)));
	strncat(command2, temp, (sizeof(command2) - strlen(command2)));
	strncat(command2, trail2, (sizeof(command2) - strlen(command2)));
	strncat(command3, temp, (sizeof(command3) - strlen(command3)));
	strncat(command3, trail3, (sizeof(command3) - strlen(command3)));
	//sprintf(temp, "%d", Spid);
	//strcat(welcome2, temp);
	//strncpy(temp, ", kill -10 ", sizeof(temp));
	//strcat(welcome2, temp);
	//sprintf(temp, "%d", Spid);
	//strcat(welcome2, temp);

	char welcome3[56] = "18 is to resume, 12 to pause, 10 to output a log.";
	char welcome4[46] = "To end, press Ctrl + C in the Output Terminal";
	setenv("welcome0", welcome0, 1);
	setenv("welcome1", welcome1, 1);
	setenv("welcome2", welcome2, 1);
	setenv("welcome3", welcome3, 1);
	setenv("welcome4", welcome4, 1);
	setenv("command1", command1, 1);
	setenv("command2", command2, 1);
	setenv("command3", command3, 1);

	// launch a new terminal (Input Terminal) that allows the user to send signals to the running processes
	int exit_status = system("gnome-terminal -- sh -c \"echo $welcome1; echo $welcome0; echo $welcome2; "
							 "echo $command1; echo $command2; echo $command3; "
							 "echo $welcome3; echo $welcome0; echo $welcome4; echo $welcome0; exec bash\"");

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