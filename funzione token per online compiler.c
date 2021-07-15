#include <stdio.h>
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

int main()
{
    float dt = 0;
    clock_t t = 0;
    float a = 0;
    for(int i = 0; i < 1000; i++)
    {
    t = clock() - t;
    dt = ((float)t) / ((float)CLOCKS_PER_SEC);
    //a = a + dt * (((float)1) - (powf(a, ((float)2)) / ((float)2))) * ((float)2) * ((float)M_PI) * ((float)100);
    a = a + dt * (1 - (powf(a, 2) / 2)) * 2 * M_PI * 1;
    printf("\n%f", a);
    usleep(1000);
    }
    return 0;
}


