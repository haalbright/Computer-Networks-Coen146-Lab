/*Name: Hanna Albright901
Date: 04/22/21
Title: Lab4 - TCP/IP Socket Programming
Description: Client connects to server and copies a file with the input given in the command line. The client uses a TCP socket.
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

int main (int argc, char *argv[]) {//need to add input for the output file to show works for all strings
    int		sockfd = 0;
    char buffc[20]; //buffer for socket
    struct	sockaddr_in servAddr;
    int fp,rbc, conn;
    int port=5008;
    int fd=socket(AF_INET, SOCK_STREAM, 0);//declare file discriptor
    if (fd < 0){//socket returns -1 if error
      perror("Failure to setup an endpoint socket");
      exit(1);
    }
    bzero(&servAddr, sizeof(servAddr));//clear
    //Set server address by filling sevAddr fields
    servAddr.sin_family = AF_INET;
    servAddr.sin_port =  htons(port);//port # to network byte order
    servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");//convert dot-decimal address
    if ((conn=connect(fd, (struct sockaddr *)&servAddr, sizeof(servAddr)))<0){//returns -1 if error
      perror("Failure to connect to the server");
      exit(1);
    }
    // Open file
    FILE *fout;
    fout=fopen(argv[1],"w");//use command line argument
    if (fout==NULL) {
      perror("Failure to open the file");
      exit(1);
    }
    int total=0;//total counts number of bytes recieved
    printf("Copying server file to output file....\n");
   while ((rbc=recv(fd, buffc, sizeof(buffc),0))>0){
      fwrite(buffc, 1, rbc, fout); // -1 to omit newline
      bzero(&buffc,sizeof(buffc));//clear buffer
      total+=rbc;
    }
    printf("Copy of %d byte file complete!\n", total);
    printf("\n");
    fclose(fout);//close file
    close(fd);//close socket
    return 0;
   }
