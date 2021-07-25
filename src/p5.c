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
    printf("\t- 1 -\n");

    double array1[sizeof(int) + 1];
    double array2[sizeof(float) + 1];
    double array3[sizeof(double) + 1];

    printf("%ld\n", sizeof(array1) / sizeof(double));
    printf("%ld\n", sizeof(array2) / sizeof(double));
    printf("%ld\n", sizeof(array3) / sizeof(double));

    printf("\t- 2 -\n");

    int a = 12;
    float b = -4.123456134234123123;
    double c = -4123456134234123456134234123456134234.112345613423412345613423412345613423423456;
    printf("a: %d\n", a);
    printf("b: %f\n", b);
    printf("c: %lf\n", c);

    int size_int = snprintf(NULL, 0, "%d", a);
    int size_float = snprintf(NULL, 0, "%f", b);
    int size_double = snprintf(NULL, 0, "%lf", c);

    printf("%d\n", size_int);
    printf("%d\n", size_float);
    printf("%d\n", size_double);

    printf("\t- 3 -\n");

    int d = 123454;
    float e = -4234234.123456123123414;
    double f = -4.123456;
    printf("d: %d\n", d);
    printf("e: %f\n", e);
    printf("f: %lf\n", f);

    int size_int2 = snprintf(NULL, 0, "%d", d);
    int size_float2 = snprintf(NULL, 0, "%f", e);
    int size_double2 = snprintf(NULL, 0, "%lf", f);

    printf("%d\n", size_int2);
    printf("%d\n", size_float2);
    printf("%d\n", size_double2);

    printf("\t- 4 -\n");

    int arr4[sizeof(int) + 1];
    printf("%ld\n", sizeof(arr4) / sizeof(int));
    printf("int: %ld\n", sizeof(int));
    printf("flt: %ld\n", sizeof(float));
    printf("dou: %ld\n", sizeof(double));

    char read1[3];
    sprintf(read1, "%d", 65);
    printf("%s\n\n", read1);
    for (size_t i = 0; i < sizeof(read1); i++)
    {
        printf("%c\n", read1[i]);
    }
    

    return 0;
}