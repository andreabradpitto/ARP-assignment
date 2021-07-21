#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/prctl.h> // non-posix?
#include "config.h"

// This process is the one responsible for logs. It registers every input received and, when prompted,
// opens the current log file via the user's preferred application


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
    char *logpath = "log.txt";
    setenv("log_file", logpath, 1);
    FILE* log_file = fopen(logpath, "w+");

    long int time_var = 0;

    struct message msg;

    while (1)
    {
        // read() is a blocking function by default (i.e. when, like here, O_NONBLOCK is not set):
        // wait here until some data is available
        read(atoi(argv[4]), &msg, sizeof(struct message));

        //holds a series of text lines in couples:
        //<timestamp> <from G | from S > <value>
        //<timestamp> <sent value> (a sample of the wave)
        switch (msg.status)
        {
            case 0: // logging acknowledgment of pause signal
                pretty_time = ctime(&msg.timestamp.tv_sec);
                pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
                time_var = msg.timestamp.tv_sec * 1000000 + msg.timestamp.tv_usec;
                log_file = fopen(logpath, "a");
                fprintf(log_file, "%li %s %s %s\n", time_var, "-", pretty_time, "- from S - pause");
                fclose(log_file);
                break;
            case 1: // logging acknowledgment of resume/continue signal
                pretty_time = ctime(&msg.timestamp.tv_sec);
                pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
                time_var = msg.timestamp.tv_sec * 1000000 + msg.timestamp.tv_usec;
                log_file = fopen(logpath, "a");
                fprintf(log_file, "%li %s %s %s\n", time_var, "-", pretty_time, "- from S - start");
                fclose(log_file);
                break;
            case 8: // logging acknowledgment of token value received by P
                pretty_time = ctime(&msg.timestamp.tv_sec);
                pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
                time_var = msg.timestamp.tv_sec * 1000000 + msg.timestamp.tv_usec;
                log_file = fopen(logpath, "a");
                fprintf(log_file, "%li %s %s %s %f\n", time_var, "-", pretty_time, "- from G - value - ", msg.value);
                fclose(log_file);
                break;
            case 9: // logging acknowledgment of token value sent by P
                pretty_time = ctime(&msg.timestamp.tv_sec);
                pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
                time_var = msg.timestamp.tv_sec * 1000000 + msg.timestamp.tv_usec;
                log_file = fopen(logpath, "a");
                fprintf(log_file, "%li %s %s %s %f\n", time_var, "-", pretty_time, "- from P - value - ", msg.value);
                fclose(log_file);
                break;
            case 3: // open log file request received
                pretty_time = ctime(&msg.timestamp.tv_sec);
                pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
                time_var = msg.timestamp.tv_sec * 1000000 + msg.timestamp.tv_usec;
                log_file = fopen(logpath, "a");
                fprintf(log_file, "%li %s %s %s\n", time_var, "-", pretty_time, "- from S - logrq");
                fclose(log_file);
                int exit_status = system("xdg-open $log_file");            
                break;
        }
    }

    fclose(log_file);
    unsetenv("log_file");
    close(atoi(argv[4]));
    return 0;
}
