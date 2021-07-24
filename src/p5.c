#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>

int main(void)
{
    double array1[sizeof(int) + 1];
    double array2[sizeof(float) + 1];
    double array3[sizeof(double) + 1];

    printf("%ld\n", sizeof(array1) / sizeof(double));
    printf("%ld\n", sizeof(array2) / sizeof(double));
    printf("%ld\n", sizeof(array3) / sizeof(double));

    printf("\n");

    int a = 12;
    float b = -4.123456;
    double c = -4.123456;

    int size_int = snprintf(NULL, 0, "%d", a);
    int size_float = snprintf(NULL, 0, "%f", b);
    int size_double = snprintf(NULL, 0, "%lf", c);

    printf("%d\n", size_int);
    printf("%d\n", size_float);
    printf("%d\n", size_double);

    return 0;
}