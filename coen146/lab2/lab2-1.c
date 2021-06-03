//Name: Hanna
//Date: 04/07/21
//Title: Lab2 - Multithreading in C and File transfer 
//Description: This code copies a file using functions and calculates the total clock ticks for the action. The duration of time to copy a file is then calculated by dividing the clock ticks by clock ticks per second.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
clock_t start, end;
double cpu_time_used;


int main(int argc, char * argv[]){
	start = clock();
	FILE *fin, *fout;
	char buf[100];
	fin = fopen(argv[1],"r");
	fout = fopen(argv[2],"w");
	while (fread(&buf, 100, 1, fin)){
  		fwrite(&buf, 100, 1,fout);
	}
	fclose(fin);
	fclose(fout);
	end = clock();
	cpu_time_used = ((double)(end - start))/CLOCKS_PER_SEC;
	printf("Time: %f seconds\n", cpu_time_used);
return 0;	
}
