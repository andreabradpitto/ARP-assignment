#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
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

// This process is the one responsible for logs. It registers every input received and, when prompted,
// outputs in the terminal all the history (log) of what happened since the beginning


int main(int argc, char *argv[])
{
    close(atoi(argv[0]));
    close(atoi(argv[1]));
    close(atoi(argv[2]));
    close(atoi(argv[3]));
    close(atoi(argv[5]));

    pid_t Lpid;
    Lpid = getpid();
    prctl(PR_SET_PDEATHSIG, SIGHUP); // asks the kernel to deliver the SIGHUP signal when parent dies, i.e. also terminates L
    printf("L: my PID is %d\n", Lpid);

    char *pretty_time;
    int logfd;
    char *logpath = "log.txt";
    logfd = open(logpath, O_CREAT | O_WRONLY);
    int bufsize = 1024;
    char buffer[bufsize];
    //ssize_t ret_in;

    struct message msg;

    while (1)
    {
        // read() is a blocking function by deafult (O_NONBLOCK is not set): wait here until some data is available
        read(atoi(argv[4]), &msg, sizeof(struct message));

        //holds a series of text lines in couples:
        //<timestamp> <from G | from S > <value>
        //<timestamp> <sent value> (a sample of the wave)
        switch (msg.status)
        {
        case 0:
            sprintf(buffer, "\n%lu%s", msg.timestamp, " - received from S - pause command issued");
            write(logfd, &buffer, sizeof(buffer)); //non so se venga riscritto tutto ogni volta sul buffer, da controllare
            break;
        case 1:
            sprintf(buffer, "\n%lu%s", msg.timestamp, " - received from S - continue command issued");
            write(logfd, &buffer, sizeof(buffer));
            break;
        case 99:
            pretty_time = ctime(&msg.timestamp);
            pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
            sprintf(buffer, "\n%s%s%f", pretty_time, " - received from G - sent value: ", msg.value);
            write(logfd, &buffer, sizeof(buffer));
            break;
        case 3:
            printf("\nUser asked to open logfile"); // cosi, ammesso che funzioni, me lo fa nello stesso terminale.
                                                    // Dovrei aprirne uno nuovo ma con la tecnica di S mi sa che non va.
            //int L_terminal = system("gnome-terminal");
            //qui metti L_terminal come std output

            /*while ((ret_in = read(logfd, &buffer, BUFSIZE)) > 0)
            {
                write(0, &buffer, BUFSIZE);
            }
            break;*/
            printf("\nThe log file is accessible at: '/tmp/test.txt'"); // FALSO!
            break;
        }

        //bzero(buffer, bufsize);  // clean the buffer by setting its content to 0
    }

    close(logfd);
    close(atoi(argv[4]));
    return 0;
}
