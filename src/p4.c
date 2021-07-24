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
    float value = 0;

    int fd;

    // FIFO file path
    char *myfifo = "myfifo";

    // Creating the named file(FIFO)
    mkfifo(myfifo, 0644); // 0666

    fd = open(myfifo, O_RDONLY);

    for (size_t i = 0; i < 3; i++)
    {
        // Read from FIFO
        read(fd, valore, sizeof(valore));
        value = atof(valore);

        // Print the read message
        printf("received value n.%ld: %f\n", i + 1, value);
    }
    close(fd);

    unlink(myfifo);

    return 0;
}
