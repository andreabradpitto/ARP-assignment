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

	int logfd;
	char *logpath = "log.txt";
	logfd = open(logpath, O_CREAT | O_WRONLY);
	char stringa[15] = "ciao";
	int numero = 4;
	int *num;
	num = &numero;
	//write(logfd, &stringa, strlen(stringa));
	write(logfd, &numero, sizeof(int));
	close(logfd);

    // Test read.
    int fd2 = open("log.txt", O_RDONLY);
    int new_numero;
    if (fd2 != -1) {
        read(fd2, &new_numero, sizeof(new_numero));
        printf("new_val = %d\n", new_numero);
        close(fd2);
	}

	//int res = 100 - 10 / 2 * 4;
	//printf("%d", res);

	int a = 11;
	float b = 3;
	float c = a;
	float d = (float)a;
	printf("a: %d, b: %f, c: %f, d: %f", a, b, c, d);
}
