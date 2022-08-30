/*Name: Hanna Albright901
Date: 04/22/21
Title: Lab4 - TCP/IP Socket Programming
Description: This program searver which waits for a client connection request, then binds and accepts client connection. Then, the output file is read to into a buffer of size 1024, and then to the socket to be given to the client to copy.
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


int main (){
  int sockfd;             //Declare socket file descriptor
  int rb, connfd;    // connection array
  int port=5000;
  char buff[1024];
  //Declare server address to which to bind for receiving messages and client address to fill in sending address
  struct sockaddr_in servAddr, clientAddr;
  //Open a TCP socket, if successful, returns a descriptor
  if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
    perror("Failure to setup an endpoint socket");
    exit(1);
  }
  bzero(&servAddr, sizeof(servAddr));
  //Set server address/ port
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(port);
  servAddr.sin_addr.s_addr =htonl(INADDR_ANY);

  //Set address/port of server endpoint for socket descriptor
  if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) != 0){
    perror("Failure to bind server address to the endpoint socket");
    exit(1);
  }
  listen(sockfd, 1);
  int addrlen=sizeof(clientAddr);
  connfd= accept(sockfd, (struct sockaddr*)&clientAddr, (socklen_t*)&addrlen);

  if (connfd<0){//doesnt accept if -1
    perror("Failure to accept connection to the client");
    exit(1);
  }
  printf("Connection Established with client: IP %s and Port %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
  FILE *toClient;
  toClient = fopen("Lab4file.txt","r"); //read from file
  if (toClient==NULL){
    printf("File cannot be opened.");
  }
  while (!feof(toClient)){
    buff[rb]='\0';//clear
    rb=fread(&buff,  1, sizeof(buff), toClient);//read from file to buffer
    send(connfd, buff, rb,0);//write from buff to socket
  }
  fclose(toClient);//close client
  close(sockfd);close socket

  return 0;
}
