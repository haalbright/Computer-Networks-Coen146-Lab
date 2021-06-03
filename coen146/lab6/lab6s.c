//Name: Hanna Albright
//Date: 05/19/2021
//Title: Lab 6 - Stop and Wait for an Unreliable Channel, with Loss
//Description: This code is used as a server to copy a file from the client using the stop and wait protocol with RDT 3.0 over UDP. Similarly to lab5, the server recieves a packet from the client, calculates and compares the checksum, and checks for the correct ACK number for the packet. However, RDT 3.0 adds in a timer on the client side so that if a packet doesn't arrive to the client in a certain amount of time it can resend it.
#include "lab6.h"

int getChecksum(Packet packet) {//longitudinal parity check
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    }

    return checksum;
}
// printPacket()
void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

//server sending ACK to the client
void serverSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    Packet packet;
    packet.header.seq_ack=seqnum;
    packet.header.len=sizeof(packet.data);
	  packet.header.cksum=getChecksum(packet);//create packet
    sendto(sockfd,&packet,sizeof(packet),0,address,addrlen);//send packet to the client
    printf("\t Server sending ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}
//server receiving packet
Packet serverReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet;
    while (1) { //while the checksum or ACK number is different from the client's or hasn't recieved yet
        recvfrom(sockfd,&packet,sizeof(packet),0,address,addrlen);
        printPacket(packet);
        int e_cksum=getChecksum(packet);
        if (packet.header.cksum!=e_cksum) {//check if checksums are different
            printf("\t Server: Bad checksum, expected checksum was: %d\n", e_cksum);
            serverSend(sockfd, address, *addrlen, !seqnum);
        } else if (packet.header.seq_ack != seqnum) {//check if ACK num are different
            printf("\t Server: Bad seqnum, expected sequence number was:%d\n",seqnum);
            serverSend(sockfd,address,*addrlen,!seqnum);
        } else {//if the ACK and checksum are correct
            printf("\t Server: Good packet\n");
            serverSend(sockfd,address,*addrlen,seqnum);
            break;
        }
    }
    return packet;
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        exit(1);
    }
    srand((unsigned)time(NULL));
    int sockfd;
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){//create socket
     perror("Failed to setup endpoint socket.\n");
     exit(1);
    }

    struct sockaddr_in servAddr; //initialize server address
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_port=htons(atoi(argv[1]));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr=INADDR_ANY;
    if(bind(sockfd,(struct sockaddr *)&servAddr,sizeof(struct sockaddr))){
      perror("Server can't bind to endpoint socket\n");
      exit(1);
    }
    int fpS=open(argv[2],O_CREAT|O_RDWR|O_TRUNC,0666);//output file
    if (fpS<0){
	    perror("File failed to open\n");
	    exit(1);
    }
    // your code to get file contents from client and write it to the file

      int seqnum = 0;
      Packet packet;
      struct sockaddr_in clientaddr;
      memset(&clientaddr, 0, sizeof(clientaddr));
      socklen_t clientaddr_len = sizeof(clientaddr);
      do {
        packet=serverReceive(sockfd,(struct sockaddr *)&clientaddr,&clientaddr_len,seqnum);
        write(fpS,packet.data,packet.header.len);//strlen(packet.data)
        seqnum=(seqnum+1)%2;//new ACK
      } while (packet.header.len != 0);//while still transmitting date
    close(fpS);//close output file
    close(sockfd);//close socket
    return 0;
}
