// 2019 ARP assignment V2.0
// Andrea Pitto - S3942710
// 10 - 07 - 2020

// This process is the one responsible of logging. It registers every command issued by the user
// and every token processed by P (received/sent). When prompted, it opens the current log file
// via the user's preferred application

#include "def.h"

int main(int argc, char *argv[])
{
    close(atoi(argv[0]));
    close(atoi(argv[1]));
    close(atoi(argv[2]));
    close(atoi(argv[3]));
    close(atoi(argv[5]));

    pid_t Lpid = getpid();
    printf("L: my PID is %d\n", Lpid);
    pid_t Parpid = getppid(); // get process ID of parent (i.e. main)

    char *fancy_time;
    char *logpath = "log.txt"; // specify log file path
    setenv("log_file", logpath, 1);
    // Open the log file file in write mode (overwrite log file if already existing, create new one otherwise)
    FILE *log_file = fopen(logpath, "w+");
    long int time_var = 0;

    struct log_message log_msg;

    while (1)
    {
        // If main is dead, end the process
        if (getppid() != Parpid)
        {
            fclose(log_file); // close the log file
            unsetenv("log_file");
            close(atoi(argv[4]));
            return 0;
        }

        // read() is a blocking function by default (i.e. when, like here, O_NONBLOCK is not set):
        // wait here until some data is available
        read(atoi(argv[4]), &log_msg, sizeof(struct log_message));

        switch (log_msg.status)
        {
        case 0: // logging acknowledgment of pause signal
            fancy_time = ctime(&log_msg.timestamp.tv_sec);
            fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s\n", time_var, "|", fancy_time, "| from S | pause | undefined");
            fclose(log_file);
            break;
        case 1: // logging acknowledgment of resume/continue signal
            fancy_time = ctime(&log_msg.timestamp.tv_sec);
            fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s\n", time_var, "|", fancy_time, "| from S | start | undefined");
            fclose(log_file);
            break;
        case 8: // logging acknowledgment of token value received by P
            fancy_time = ctime(&log_msg.timestamp.tv_sec);
            fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s %9f\n", time_var, "|", fancy_time, "| from G | value |", log_msg.value);
            fclose(log_file);
            break;
        case 9: // logging acknowledgment of token value sent by P
            fancy_time = ctime(&log_msg.timestamp.tv_sec);
            fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s %9f\n", time_var, "|", fancy_time, "| from P | value |", log_msg.value);
            fclose(log_file);
            break;
        case 3: // open log file request received
            fancy_time = ctime(&log_msg.timestamp.tv_sec);
            fancy_time[strcspn(fancy_time, "\n")] = 0; // remove newline from ctime() output
            time_var = log_msg.timestamp.tv_sec * 1000000 + log_msg.timestamp.tv_usec;
            log_file = fopen(logpath, "a");
            fprintf(log_file, "%li %s %s %s\n", time_var, "|", fancy_time, "| from S | logrq | undefined");
            fclose(log_file);
            int exit_status = system("xdg-open $log_file");
            break;
        }
    }

    fclose(log_file); // close the log file
    unsetenv("log_file");
    close(atoi(argv[4]));
    return 0;
}
