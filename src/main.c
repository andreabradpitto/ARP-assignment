// 2019 ARP assignment V2.0
// Andrea Pitto - S3942710
// 10 - 07 - 2020

// This is the main process, which launches all the other software. The duty of this piece of code
// is to load the 3 unnamed pipes and to launch all the involved processes (S, P, G and L)

#include "def.h"

void interrupt_handler(int signum)
{
	kill(atoi(getenv("Spid")), SIGKILL);
	kill(atoi(getenv("Gpid")), SIGKILL);
	kill(atoi(getenv("Ppid")), SIGKILL);
	kill(atoi(getenv("Lpid")), SIGKILL);
	unsetenv("Spid");
	unsetenv("Gpid");
	unsetenv("Ppid");
	unsetenv("Lpid");
	_exit(0);
}

int main(int argc, char *argv[])
{
	pid_t P, G, S, L;

	int pfd1[2]; // file descriptors (a.k.a. fd) for pipe 1 (S writes on it, P reads from it)
	int pfd2[2]; // file descriptors (a.k.a. fd) for pipe 2 (G writes on it, P reads from it)
	int pfd3[2]; // file descriptors (a.k.a. fd) for pipe 3 (P writes on it, L reads from it)

	int wait_status = 0;
	char pid_buf[10];

	if (pipe(pfd1) < 0) // error condition for pipe 1
	{
		perror("\nPipe 1 creation error");
		return -1;
	}
	if (pipe(pfd2) < 0) // error condition for pipe 2
	{
		perror("\nPipe 2 creation error");
		return -1;
	}
	if (pipe(pfd3) < 0) // error condition for pipe 3
	{
		perror("\nPipe 3 creation error");
		return -1;
	}

	// Creating char arrays for the unnamed pipes
	char read1[3]; // size 2 should be also fine
	char write1[3];
	char read2[3];
	char write2[3];
	char read3[3];
	char write3[3];

	// Load each pipe's fd read/write end (3rd arg.) into the char array (1st arg.), while formatting it as stated in 2nd arg.
	sprintf(read1, "%d", pfd1[0]);
	sprintf(write1, "%d", pfd1[1]);
	sprintf(read2, "%d", pfd2[0]);
	sprintf(write2, "%d", pfd2[1]);
	sprintf(read3, "%d", pfd3[0]);
	sprintf(write3, "%d", pfd3[1]);

	// Preparing to send all pipe ends to the children of this process, by transforming their fd in char.
	// They will be reverted back to integers inside those children
	argv[0] = read1;  // pipe1: read
	argv[1] = write1; // pipe1: write
	argv[2] = read2;  // pipe2: read
	argv[3] = write2; // pipe2: write
	argv[4] = read3;  // pipe3: read
	argv[5] = write3; // pipe3: write

	signal(SIGINT, interrupt_handler); // reacts to "Ctrl+C"

	S = fork();
	sprintf(pid_buf, "%d", (int)S);
	setenv("Spid", pid_buf, 1);

	if (S < 0) // error condition on fork
	{
		perror("\nFork S");
		return -1;
	}

	if (S == 0) // S process
	{
		char *node_name = "./S";
		if (execvp(node_name, argv) < 0) // error handling for S process
		{
			perror("\nExec failed for S");
			return -1;
		}
	}

	else if (S > 0)
	{
		G = fork();
		sprintf(pid_buf, "%d", (int)G);
		setenv("Gpid", pid_buf, 1);

		if (G < 0) // error condition on fork
		{
			perror("\nFork G");
			return -1;
		}

		if (G == 0) // G process
		{
			char *node_name = "./G";
			if (execvp(node_name, argv) < 0) // error handling for G process
			{
				perror("\nExec failed for G");
				return -1;
			}
		}
		else if (G > 0)
		{
			L = fork();
			sprintf(pid_buf, "%d", (int)L);
			setenv("Lpid", pid_buf, 1);

			if (L < 0) //error condition on fork
			{
				perror("\nFork L");
				return -1;
			}

			if (L == 0) // L process
			{
				char *node_name = "./L";
				if (execvp(node_name, argv) < 0) //error handling for L process
				{
					perror("\nExec failed for L");
					return -1;
				}
			}
			else if (L > 0)
			{
				P = fork();
				sprintf(pid_buf, "%d", (int)P);
				setenv("Ppid", pid_buf, 1);

				if (P < 0) // error condition on fork
				{
					perror("\nFork P");
					return -1;
				}

				if (P == 0) // P process
				{
					char *node_name = "./P";
					if (execvp(node_name, argv) < 0) // error handling for P process
					{
						perror("\nExec failed for P");
						return -1;
					}
				}
			}
		}

		close(pfd1[0]);
		close(pfd1[1]);
		close(pfd2[0]);
		close(pfd2[1]);
		close(pfd3[0]);
		close(pfd3[1]);

		// Welcome the user with useful information
		printf("\n[This is the Output Terminal, which shows the processes' outputs. "
			   "Use the Input Terminal to send signals to the processes. "
			   "Press Ctrl+C to end]\n\n");

		printf("main: PID is %d\n", getpid());

		wait(&wait_status); // waits for any child to die before proceeding; equivalent to: waitpid(-1, &wait_status, 0);
		unsetenv("Spid");
		unsetenv("Gpid");
		unsetenv("Ppid");
		unsetenv("Lpid");
		return 0;
	}
}
