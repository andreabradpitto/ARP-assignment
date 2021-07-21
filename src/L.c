// 2019 ARP assignment V2.0
// Andrea Pitto - S3942710
// 10 - 07 - 2020

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/types.h>
#include "config.h"

// This process is the one responsible of logging. It registers every command issued by the user
// and every token processed by P (received/sent). When prompted, it opens the current log file
// via the user's preferred application

int main(int argc, char *argv[])
{
    close(atoi(argv[0]));
    close(atoi(argv[1]));
    close(atoi(argv[2]));
    close(atoi(argv[3]));
    close(atoi(argv[5]));

    pid_t Lpid;
    Lpid = getpid();
    printf("L: my PID is %d\n", Lpid);

    char *pretty_time;
    char *logpath = "log.txt";
    setenv("log_file", logpath, 1);
    FILE *log_file = fopen(logpath, "w+");

    long int time_var = 0;

    struct log_message log_msg;

    while (1)
    {
        // read() is a blocking function by default (i.e. when, like here, O_NONBLOCK is not set):
        // wait here until some data is available
        read(atoi(argv[4]), &log_msg, sizeof(struct log_message));

        switch (log_msg.status)
        {
        case 0: // logging acknowledgment of pause signal
            pretty_time = ctime(&log_msg.timestamp.tv_sec);
            pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s\n", time_var, "|", pretty_time, "| from S | pause | undefined");
            fclose(log_file);
            break;
        case 1: // logging acknowledgment of resume/continue signal
            pretty_time = ctime(&log_msg.timestamp.tv_sec);
            pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s\n", time_var, "|", pretty_time, "| from S | start | undefined");
            fclose(log_file);
            break;
        case 8: // logging acknowledgment of token value received by P
            pretty_time = ctime(&log_msg.timestamp.tv_sec);
            pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s %9f\n", time_var, "|", pretty_time, "| from G | value |", log_msg.value);
            fclose(log_file);
            break;
        case 9: // logging acknowledgment of token value sent by P
            pretty_time = ctime(&log_msg.timestamp.tv_sec);
            pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s %9f\n", time_var, "|", pretty_time, "| from P | value |", log_msg.value);
            fclose(log_file);
            break;
        case 3: // open log file request received
            pretty_time = ctime(&log_msg.timestamp.tv_sec);
            pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s\n", time_var, "|", pretty_time, "| from S | logrq | undefined");
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
