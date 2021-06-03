//Name: Hanna Albright
//Date: 05/12/2021
//Title: Lab 5 - Stop and Wait for an Unreliable Channel
//Description: This code is used as a client to copy a file to the server using the stop and wait protocol over UDP. The client connects to the server, and reads from the input file. Before sending the data from the input file to the server through the socket, the checksum is calculated and added to the header, and I included a 25% chance of a checksum error using rand. It waits for the server to respond, and if the same ACK number or checksum is recieved, then continue reading from the input file. Else, resend the packet.
#include "lab5.h"

/*The simplest checksum algorithm is the so-called longitudinal parity check,
which breaks the data into "bytes" with a fixed number 8 of bits, and then
computes the exclusive or (XOR) of all those bytes.
The result is appended to the message as an extra byte*/
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
// printPacket()
void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}
//clientSend()
void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet) {
    while (1) {
		    packet.header.cksum=getChecksum(packet);
        if (rand()%5==0){//chance of error
           packet.header.cksum=0; //  //simulate checksum error
           printf("\n CHECKSUM ERROR SIMULATED \n");
        }
        // send the packet
        printf("Client sending packet\n");
        sendto(sockfd,&packet,sizeof(packet),0,address,addrlen);
        Packet recvpacket;
        recvfrom(sockfd,&recvpacket,sizeof(recvpacket),0,(struct sockaddr*)address,&addrlen);
        printf("Client received ACK %d, checksum %d - \n", recvpacket.header.seq_ack, recvpacket.header.cksum);
        int e_cksum=getChecksum(recvpacket); //get the checksum

        if (recvpacket.header.cksum!=e_cksum ) {//if the checksum isn't the same, resend packet
            printf("Client: Bad checksum, expected checksum was: %d\n", e_cksum);
        } else if (recvpacket.header.seq_ack!=packet.header.seq_ack) {//if the ACK number is incorrect, resend packet
            printf("Client: Bad seqnum, expected sequence number was: %d\n",packet.header.seq_ack);
        } else {
            // good ACK, we're done
            printf("Client: Good ACK\n");
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }

    // your code to create and configure the socket [name the socket sockfd]
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
    address.sin_port=htons(atoi(argv[2]));
    address.sin_family=AF_INET;
    address.sin_addr=*((struct in_addr *)host->h_addr);
   int fp=open(argv[3], O_RDWR);
   if (fp<0){
    	perror("Failed to open file\n");
	    exit(1);
    }

    // your code to send file contents to server
    int seq=0;
    Packet pack;
    int rd;
    while((rd=read(fp, pack.data, sizeof(pack.data)))>0){//while # of bits read > 0
    	pack.header.seq_ack=seq;
    	pack.header.len=rd;
    	pack.header.cksum=getChecksum(pack);//create packet
    	clientSend(sockfd, (struct sockaddr *)&address,addr_len,pack);//send to server
      bzero(&pack.data,pack.header.len);
    	seq=(seq+1)%2;//new ACK number calculated
    }

    // send zero-length packet to server to end connection
    Packet final;
    final.header.seq_ack=seq;
    final.header.len=0;
    final.header.cksum=getChecksum(final);
    clientSend(sockfd,(struct sockaddr *)&address,addr_len,final);
    close(fp); //close input file
    close(sockfd);//close socket
    return 0;
}
