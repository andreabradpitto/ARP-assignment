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

#define BUFSIZE 1024

//This node is the one responsible for logs. It registers every input received and, when prompted,
//outputs in the terminal all the history (log) of what happened since the beginning

/*
void timestamp()
{
    time_t ltime; //calendar time
    ltime=time(NULL); //get current cal time
    printf("%s",asctime( localtime(&ltime) ) );
}
*/

//mi sa che questo sotto è da cancellare, il timestamp lo faccio in P insieme al resto
//questo può essere inutile, dipende come gestisco il dato se uso la struct definita in config.h, credo
/*
char* time_stamp() //va bene così?? ...char* asctime (const struct tm * timeptr); http://www.cplusplus.com/reference/ctime/asctime/
{
    time_t ltime;       // calendar time
    ltime = time(NULL); // get current cal time
    return asctime(localtime(&ltime)));
}
*/

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

    /*
    int logfd;
    char *logfile = "/home/andrea/Documenti/Università/Advanced and robot programming/Assignment/tmp/log_file"; // da adattare a Ubuntu
    logfd = open(logfile, O_WRONLY);
    */

    int logfd;
    char *logpath = "/dev/log/PROVA";
    logfd = open(logpath, O_RDONLY);
    char buffer[BUFSIZE]; //char buffer[BUFSIZE] = ""; //ma ho già il buffer nel main, uso quello?
    ssize_t ret_in;

    struct message msg;

    openlog("[Assignment]", 0, LOG_USER);

    while (1)
    {
        read(atoi(argv[4]), &msg, sizeof(struct message));

        //holds a series of text lines in couples:
        //<timestamp> <from G | from S > <value>
        //<timestamp> <sent value> (a sample of the wave)
        switch (msg.status)
        {
        case 0:
            //prima era: syslog(LOG_INFO, "%s%s", time_stamp(), " from S pause command issued\n");
            syslog(LOG_INFO, "%lu%s", msg.timestamp, " - from S - pause command issued\n");
            break;
        case 1:
            syslog(LOG_INFO, "%lu%s", msg.timestamp, " - from S - continue command issued\n");
            break;
        case 99:
            syslog(LOG_INFO, "%lu%s%f%s", msg.timestamp, " - from G - sent value: ", msg.value, "\n");
            break;
        case 3:
            //print in output del file di log
            printf("[User asked print of logfile\n]");
            while ((ret_in = read(logfd, &buffer, BUFSIZE)) > 0)
            {
                write(0, &buffer, BUFSIZE);
            }
            break;
        }
    }
    closelog();
    close(atoi(argv[4]));
    return 0;
}