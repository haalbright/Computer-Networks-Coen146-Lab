//Name: Hanna Albright
//Date: 05/19/2021
//Title: Lab 6 - Stop and Wait for an Unreliable Channel, with Loss
//Description: This code is used as a client to copy a file from the client using the stop and wait protocol with RDT 3.0 over UDP. Similarly to lab5, the client sends a packet to the server which has the checksum, ACK number, and the input file data.RDT 3.0 adds in a timer on the client side which starts as soon as a a packet is sent and if a response isn't recieved when the timer runs out, the packet is resent (packet can be sent up to 3 times total).

#include "lab6.h"

//getChecksum()
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    }
        return checksum;
}

void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) {
    while (1) {
        //if retries is greater than 3, we give up and move on
	    if(retries>=3){
		    break;
	    }
        // calculate the checksum
        packet.header.cksum=getChecksum(packet);
        // Print what is being sent
        printf("Created: ");
        printPacket(packet);

        // Simulate lost packet.
        if (rand() % 8 == 0) {
            printf("Dropping packet\n");
        } else {
          sendto(sockfd,&packet,sizeof(packet),0,address,addrlen);
        }
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int rv;
        //setup select parameters
        fd_set readfds;
        fcntl(sockfd,F_SETFL,O_NONBLOCK);
        //start before calling select
        FD_ZERO(&readfds);
        FD_SET(sockfd,&readfds);//set up file descriptor and socket
        //call select. sockfd+1 is used because select checks upto sockfd-1 descriptors.
        rv=select(sockfd+1,&readfds,NULL,NULL,&tv);
        if(rv==0){ // no data has been received
            printf("Timeout\n");
            retries++;//increment retries if packet is dropped
        }
        else{ //if rv!=0, there is something to receive
          Packet recvpacket;
          recvfrom(sockfd,&recvpacket,sizeof(recvpacket),0,(struct sockaddr*)address,&addrlen);
        // receive an ACK from the server

          printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);
          int e_cksum=getChecksum(recvpacket); //get the checksum

          // validate the ACK
          if (recvpacket.header.cksum!=e_cksum ) {//if the checksum isn't the same, resend
              printf("Client: Bad checksum, expected checksum was: %d\n", e_cksum);
          }
          else if (recvpacket.header.seq_ack!=packet.header.seq_ack) {//if the ACK number is incorrect, resend
              printf("Client: Bad seqnum, expected sequence number was: %d\n",packet.header.seq_ack);
          }
          else {
              // good ACK, we're done
              printf("Client: Good ACK\n");
              break;
          }
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }

    // seed the RNG
    srand((unsigned)time(NULL));
    int sockfd;
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){
	    perror("Failed to setup endpoint socket.\n");
	    exit(1);
    }

    // your code to inialize server address structure using argv[2] and argv[1]
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    socklen_t addr_len=sizeof(address);
    struct hostent* host;
    host=(struct hostent *)gethostbyname(argv[1]);
    address.sin_port=htons(atoi(argv[2]));//port #
    address.sin_family=AF_INET;
    address.sin_addr=*((struct in_addr *)host->h_addr);
   int fp=open(argv[3], O_RDWR);//open input file
   if (fp<0){
    	perror("Failed to open file\n");
	    exit(1);
    }
    // send file contents to server
    int seq=0;
    Packet pack;
    int rd;
    while((rd=read(fp, pack.data, sizeof(pack.data)))>0){//while # of bits read > 0
    	pack.header.seq_ack=seq;
    	pack.header.len=rd;
    	pack.header.cksum=getChecksum(pack);//create packet
    	clientSend(sockfd, (struct sockaddr *)&address,addr_len,pack,0);//send to server
      bzero(&pack.data,pack.header.len);
    	seq=(seq+1)%2;//new ACK number calculated
    }

    // send zero-length packet to server to end connection according the description provided in the Lab manual
    Packet final;
    final.header.seq_ack=seq;
    final.header.len=0;
    final.header.cksum=getChecksum(final);
    clientSend(sockfd,(struct sockaddr *)&address,addr_len,final,0);
    // clean up
    close(fp);
    close(sockfd);
    return 0;
}
