#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "config.h"

// This is the process that is used to communicate with the terminal. It receives and handles 3 different commands: start, pause, log.
// In order to use it, you have to type e.g. "kill -9 1234", where the first number is the signal chosen, whilst the second
// is the PID of the node S. You only interact with this process, which in turn communicates with P, and so on.


// These 3 flags are stored as global variables because you cannot pass any argument of choice to the signal handlers
int start_flag = 0;
int stop_flag = 0;
int log_flag = 0;

// This handler is devoted to resuming token computation, as well as all other tasks in all the nodes
void start_handler(int signum)
{
	if (signum == SIGCONT)
	{
		start_flag = 1;
	}
}

// This handler is devoted to manage pause the execution of the nodes
void stop_handler(int signum)
{
	if (signum == SIGUSR2)
	{
		stop_flag = 1;
	}
}

// This handler issues the opening of the log file
void log_handler(int signum)
{
	if (signum == SIGUSR1)
	{
		log_flag = 1;
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
	printf("S: my PID is %d\n", Spid);

	// Creation of Input Terminal welcome message, including dynamic Spid injection
	char welcome0[1] = "";
	char welcome1[146] = "[This is the Input Terminal, through which you can send signals to the running processes. "
						 "Please also che the Output Terminal to inspect outputs]";
	char welcome2[20] = "Available commands:";
	char command1[30] = "kill -10 ";
	char command2[30] = "kill -12 ";
	char command3[30] = "kill -18 ";
	char Spid_array[20];
	sprintf(Spid_array, "%d", Spid);
	strncat(command1, Spid_array, (sizeof(command1) - strlen(command1)));
	strncat(command2, Spid_array, (sizeof(command2) - strlen(command2)));
	strncat(command3, Spid_array, (sizeof(command3) - strlen(command3)));
	char welcome3[96] = "10 is to output a log, 12 to pause, 18 to resume. To end, press Ctrl + C in the Output Terminal";
	setenv("welcome0", welcome0, 1);
	setenv("welcome1", welcome1, 1);
	setenv("welcome2", welcome2, 1);
	setenv("welcome3", welcome3, 1);
	setenv("command1", command1, 1);
	setenv("command2", command2, 1);
	setenv("command3", command3, 1);

	// Launch a new terminal (Input Terminal) that allows the user to send signals to the running processes
	// while welcoming the user with useful information
	int exit_status = system("gnome-terminal -- sh -c \"echo $welcome0; echo $welcome1; echo $welcome0; "
							 "echo $welcome2; echo $welcome0; echo $command1; echo $command2; echo $command3; "
							 "echo $welcome0; echo $welcome3; echo $welcome0; exec bash\"");

	signal(SIGCONT, start_handler);	// reacts to "kill -18 Spid"
	signal(SIGUSR2, stop_handler);	// reacts to "kill -12 Spid"
	signal(SIGUSR1, log_handler);	// reacts to "kill -10 Spid"

	while (1)
	{
		if (start_flag)
		{
			printf("\n\nReceived start command by user\n\n");
			write(atoi(argv[1]), &(int){1}, sizeof(int)); // state = 0: P and G communication stopped
			start_flag = 0;
		}
		if (stop_flag)
		{
			printf("\n\nReceived stop command by user\n\n");
			write(atoi(argv[1]), &(int){0}, sizeof(int)); // state = 1: P and G communication resumed/in progress
			stop_flag = 0;
		}
		if (log_flag)
		{
			printf("\n\nReceived open log command by user\n\n");
			write(atoi(argv[1]), &(int){3}, sizeof(int)); // send open log file request
			log_flag = 0;
		}
	}

	unsetenv("welcome0");
	unsetenv("welcome1");
	unsetenv("welcome2");
	unsetenv("welcome3");
	unsetenv("command1");
	unsetenv("command2");
	unsetenv("command3");
	close(atoi(argv[1]));
	return 0;
}
