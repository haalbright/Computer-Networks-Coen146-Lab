//Name: Hanna Albright
//Date: 04/01/21
//Title: Lab1 - Unix/Linux Commands and Overview of C Programming
//Description: This program creates two threads that prints out 0-99. After a number is printed out, there is a delay of n microseconds where n is taken as an input from the user. If the fork fails, an error message will print showing the standard error message, "Error creating thread" message, and the error number. 
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

void *firstthread(char *argv){ //function for the first thread
	int i,n=atoi(argv); //takes in the number of microseconds as n
        for (i=0;i<100;i++){
                printf("Thread 1 %d\n",i);
                usleep(n);
        }
	return NULL;
}

void *secondthread(char *argv){ //function for the second thread
	int i,n=atoi(argv);//takes in the number of microseconds as n
	for (i=0;i<100;i++){
		printf("Thread 2 %d\n",i);
		usleep(n);
	}
	return NULL;
}

int main(int argc, char *argv[]){
	pthread_t first, second;
	int err1, err2;
	err1 = pthread_create(&first, NULL, firstthread, argv[1]); //create first thread and take argv[1] as input to firstthread function
	if(err1){
		printf("Error creating thread 1 \n", errno);
	}
	err2 = pthread_create(&second, NULL, secondthread, argv[1]); //create second thread and take argv[1] as input to secondthread function
	if(err2){//if error, print error message
                printf("Error creating thread 2 \n", errno);
        }
	pthread_join(first, NULL);//join threads
	pthread_join(second, NULL);
	return 0;
}
