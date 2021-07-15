#define _XOPEN_SOURCE 600
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <fcntl.h>

//https://stackoverflow.com/questions/16073136/how-to-open-new-terminal-through-c-program-in-linux

int main()
{
    pid_t i;
    char buf[10];
    int fds, fdm, status;

    fdm = posix_openpt(O_RDWR);
    grantpt(fdm);
    unlockpt(fdm);

    close(0);
    close(1);
    close(2);

    i = fork();
    if (i != 0)
    { // father
        dup(fdm);
        dup(fdm);
        dup(fdm);
        printf("Where do I pop up?\n");
        sleep(2);
        printf("Where do I pop up - 2?\n");
        waitpid(i, &status, 0);
    }
    else
    { // child
        fds = open(ptsname(fdm), O_RDWR);
        dup(fds);
        dup(fds);
        dup(fds);
        strcpy(buf, ptsname(fdm));
        sprintf(buf, "xterm -S%c/2", basename(buf));
        system(buf);
        exit(0);
    }
}