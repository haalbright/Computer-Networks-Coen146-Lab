/*Name: Hanna Albright901
Date: 04/22/21
Title: Lab4 - TCP/IP Socket Programming
Description: This program creates a server that listens and queues 10 client requests. The server binds and connects then creates a thread for one connection. This programm allows for five concurrent threads to copy a file by reading it iinto a buffer and sending it over the socket.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>

#define NTHREADS 5 // 5 threads
#define CONN 5 // 5 connections

void *threadF(void *arg); // thread function
pthread_t threads[NTHREADS]; // thread ID array

struct d { 		//structure to hold thread data
  int con; //connection
  FILE* fP; //file pointer
  int tID; //thread id
};

int main (){
   int	sockfd;             //Declare socket file descriptor
   int k, connfd[CONN];    // connection array
   int port=5000;
   //Declare server address to which to bind for receiving messages and client address to fill in sending address
   struct	sockaddr_in servAddr, clientAddr;
  bzero(&servAddr, sizeof(servAddr));
  bzero(&servAddr, sizeof(clientAddr));
   //Set server address/ port
   servAddr.sin_family = AF_INET;
   servAddr.sin_port = htons(port);//port # to network byte order
   servAddr.sin_addr.s_addr =htonl(INADDR_ANY);

   //Open a TCP socket, if successful, returns a descriptor
   if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0 ){//create file descriptor for socket
     perror("Failure to setup an endpoint socket");
     exit(1);
   }

   //Set address/port of server endpoint for socket descriptor
   if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr))< 0){//bind to socket
     perror("Failure to bind server address to the endpoint socket");
     exit(1);
   }


   listen(sockfd, 10);//listens and queue up to 10 requests
   k = 0;
   int addrlen = sizeof(struct sockaddr_in); //size of sockaddr_in
   //Server loops all the time accepting conncections when they are made, then passing connfd to a thread
   while (1) {
       //Server accepting to establish a connection with a connecting client, if successful, returns a connection descriptor
       connfd[k]= accept(sockfd, (struct sockaddr *)&clientAddr, (socklen_t *)&addrlen);
       if (connfd[k]<0){
         perror("Failure to accept connection to the client");
         exit(1);
       }
       FILE *toClient;
       toClient = fopen("Lab4file.txt","r"); //open file to copy
       if (toClient==NULL){
         printf("File cannot be opened.");
         exit(1);
       }
       //Connection established, server begins to read and write to the connecting client
       printf("Connection Established with client: IP %s and Port %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
       pthread_attr_t attr;
       pthread_attr_init(&attr);
       struct d *data = (struct d *) malloc(sizeof(struct d)); //Data object to pass to thread
       data->con = connfd[k];
       data->fP= toClient;
       data->tID = k;
       pthread_create(&threads[k], &attr, threadF, data); //Thread to handle connfd
       k++;
}
   close(sockfd);//close socket fd
   return 0;
}

//Thread function handling client connection
void *threadF(void *arg){
 struct d *data=arg;
 int rb;
 char buff[20];
 printf("Hello from thread %D \n", data->tID);
 sleep(10);
 while ((rb=fread(buff, 1, sizeof(buff), data->fP))>0){//(rb=fread(&buff,  sizeof(buff), 1, data->fP))>0
   printf("Bytes read from file: %d \n", rb);
   send(data->con, buff, rb,0);//write from buff to socket
   bzero(&buff,sizeof(buff));
 }
fclose(data->fP);//close output file
close(data->con);
pthread_exit(0);
}
