/*Name: Hanna Albright901
Date: 04/22/21
Title: Lab4 - TCP/IP Socket Programming
Description: This program creates a client connection via TCP to the server to copy a file with the input given in the command line. The information transmitted from the server gets written to a buffer, and then an output file.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

int main () {
  int port=5000;
  struct sockaddr_in servAddr, clientAddr;
  char buff[1024];//buffer
  int fd=socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0){
    perror("Failure to setup an endpoint socket");
    exit(1);
  }
  bzero(&servAddr, sizeof(servAddr));//clear data

  //Set server address by filling sevAddr fields
  servAddr.sin_family = AF_INET;
  servAddr.sin_port =  htons(port);
  servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

  if (connect(fd, (struct sockaddr *)&servAddr, sizeof(servAddr))!=0){
    perror("Failure to connect to the server");
    exit(1);
  }
  // Open file
  int rbc=0;//bytes recieved
  FILE *fout;
  fout=fopen("Output_.txt","w");//opent file to write to
  if (fout==NULL) {
    perror("Failure to open the file");
    exit(1);
  }
  while ((rbc=recv(fd, buff, sizeof(buff), 0))>0){
    printf("Buffer: %s \n", buff);
    fwrite(&buff, 1, rbc, fout); // -1 to omit newline
  }
  fclose(fout);//close file
  close(fd);//close socket
  return 0;
}
