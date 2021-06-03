//Name: Hanna
//Date: 04/07/21
//Title: Lab2 - Multithreading in C and File transfer 
//Description: This program creates five threads which each copy a file by reading to a buffer, and writing from the buffer to a newly created file. The five threads are joined after they finish.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define threadNum 5

void *go(char *argv){
	printf("in go");
	FILE *fin,*fout;
        char buf[100];
	char outputname[100];
  	printf("before");
	sprintf(outputname, "dest%d.dat", rand()%50);
        printf("after");
	printf("Output file: %s\n", outputname);
	fin = fopen(argv,"r");
        fout = fopen(outputname,"w");
        while (fread(&buf, 100, 1, fin)){
                fwrite(&buf, 100, 1,fout);
        }
        fclose(fin);
        fclose(fout);
return NULL;
}

int main(int argc, char *argv[]){
	int i;
	pthread_t threads[threadNum];
	printf("before create");
	for (i = 0; i < threadNum; i++)  {
        	pthread_create(&threads[i], NULL, go, argv[1]);
	}
	for (i = 0; i < threadNum; i++)  {
                pthread_join(threads[i], NULL);
        }
return 0;	
}
