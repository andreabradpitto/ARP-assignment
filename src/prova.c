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
#include <sys/time.h> //questo e il primo degli aggiunti
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <math.h>
#include "def.h"

int main(int argc, char *argv[])
{

	/*// C program to illustrate fopen()

	//#include <stdio.h>
	//#include <stdlib.h>

	int main()
	{

		// pointer demo to FILE
		FILE* demo;

		// Creates a file "demo_file"
		// with file acccess as write-plus mode
		demo = fopen("demo_file.txt", "w+");

		// adds content to the file
		fprintf(demo, "%s %s %s", "Welcome",
				"to", "GeeksforGeeks");

		// closes the file pointed by demo
		fclose(demo);

		return 0;
	}*/

	/*// C program to illustrate fopen()

	//#include <stdio.h>

	int main()
	{

		// pointer demo to FILE
		FILE* demo;
		int display;

		// Creates a file "demo_file"
		// with file acccess as read mode
		demo = fopen("demo_file.txt", "r");

		// loop to extract every characters
		while (1) {
			// reading file
			display = fgetc(demo);

			// end of file indicator
			if (feof(demo))
				break;

			// displaying every characters
			printf("%c", display);
		}

		// closes the file pointed by demo
		fclose(demo);

		return 0;
	}*/

	int logfd;
	char *logpath = "log_prova.txt";
	//logfd = open(logpath, O_CREAT | O_WRONLY);
	FILE *file;
	file = fopen(logpath, "w+");
	//char stringa[15] = "ciao";
	char stringa[100];
	long int time_var = 33222111234567890;
	fprintf(file, "%li%s\n", time_var, " - received from S - pause command issued");
	int numero = 4;
	//int *num;
	//num = &numero;
	//write(file, &stringa, strlen(stringa));
	//write(logfd, &numero, sizeof(int));
	//char stringa2[20] = " gatti";
	char stringa2[120];
	time_var = 33222119876543210;
	char *pretty_time;
	struct timeval timestamp;
	gettimeofday(&timestamp, NULL);
	pretty_time = ctime(&timestamp.tv_sec);
	pretty_time[strcspn(pretty_time, "\n")] = 0; // remove newline from ctime() output
	float value = 1.41;
	fprintf(file, "%li%s%s%s%f\n", time_var, " - ", pretty_time, " - received from G - sent value: ", value);
	//write(file, &stringa2, strlen(stringa2));
	fclose(file);

	// Test read.
	FILE *file2;
	file2 = fopen(logpath, "r");
	char output_stringa[1000];
	//int new_numero;
	int display;
	/*if (file2 != -1) {
        //read(fd2, &new_numero, sizeof(new_numero));
        //printf("new_val = %d\n", new_numero);
        read(file2, &output_stringa, sizeof(output_stringa));
        printf("\noutput_stringa = %s\n", output_stringa);
        fclose(file2);
	}*/
	while (1)
	{
		display = fgetc(file2);
		if (feof(file2)) // end of file indicator
			break;
		printf("%c", display);
	}
	fclose(file2);

	//int res = 100 - 10 / 2 * 4;
	//printf("%d", res);

	int a = 11;
	float b = 3;
	float c = a;
	float d = (float)a;
	printf("a: %d, b: %f, c: %f, d: %f", a, b, c, d);

	float e = 12.5 - a;
	printf("\ne: %f", e);

	time_t tempo;
	tempo = time(NULL);
	printf("\n%li", tempo);
	sleep(1);
	time_t *tempoPtr = &tempo;
	time(tempoPtr);
	//*tempoPtr = time(NULL);
	printf("\n%li", *tempoPtr);

	float numer = 4.3;
	int denom = 2;
	float ris = numer / denom;
	printf("\nris: %f", ris);

	printf("\n\nTEMPO\n");
	struct timeval token_timestamp;
	//clock_gettime(0, token_timestamp);
	//time_t *pointer = &(token_timestamp->tv_sec);
	//printf("%li", *pointer);
	gettimeofday(&token_timestamp, NULL);
	printf("%li\n", token_timestamp.tv_sec);

	char *prova = "ciao";
	prova = "miaodaaS";
	for (int i = 0; i < sizeof(prova); i++)
		printf("%c\n", prova[i]);

	printf("\n\n");

	char lunghezza[2];
	//char l2[10] = "-1.123456";
	char l2[9];
	int numerone = 1234567890;
	float numerotto = 1.23456789000;
	sprintf(lunghezza, "%d", numerone);
	printf("%s", lunghezza);
	sprintf(l2, "%f", numerotto);
	printf("%s", l2);
	float cat = atof(l2);
	printf("\n%f\n", cat);

	printf("\nfloat = %li; double = %li\n", sizeof(float), sizeof(double));
}
