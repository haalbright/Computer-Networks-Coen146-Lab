//Name: Hanna
//Date: 04/07/21
//Title: Lab2 - Multithreading in C and File transfer 
//Description: This program uses system calls to copy a file and calculate the time needed to do so by dividing the total clock ticks by clocks per second.
#include <sys/stat.h
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
	int fin, fout;
	char buf[100];
	fin = open(argv[1],O_RDWR);
	fout = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC, 0644);
	while (read(fin,buf, sizeof(buf))){
  		write(fout, buf, sizeof(buf));
	}
	close(fin);
	close(fout);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time: %f seconds\n", cpu_time_used);
}
