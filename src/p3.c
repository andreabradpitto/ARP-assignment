#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    char valore[sizeof(double) + 1];
    //char valore[sizeof(double) + 1] = "5.34";
    float value = 5.34;

    int fd;

    // FIFO file path
    char *myfifo = "myfifo";

    // Creating the named file(FIFO)
    mkfifo(myfifo, 0644); // 0666

    fd = open(myfifo, O_WRONLY);

    for (size_t i = 0; i < 3; i++)
    {
        sprintf(valore, "%f", value);
        //value = atof(valore);

        write(fd, valore, sizeof(valore));

        // Print the read message
        printf("sent value n.%ld: %f\n", i + 1, value);
        value = value + 1;
    }
    close(fd);

    unlink(myfifo);

    return 0;
}
