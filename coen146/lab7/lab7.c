// Name: Hanna Albright
// Date: 06/2/21
// Title: Lab7: Link State routing
// Description: This code takes in a list of machines (nodes) and their cost table to create a network using the Dijkstra algorithm. The network has 4 nodes, where each node is initially connected to only two other nodes with a cost of 1. Each node asks if any costs need to be added and if so, it updates the costs array and sends the sender and recieve node numbers and the new cost to the other nodes so they each can find update their tables to have the current the least cost path to each node.

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>


//defines
#define	N			4
#define	INFINITE	1000
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))


// types
typedef struct machines
{
	char	name[50];
	char	ip[50];
	int		port;

} MACHINES;


// global variables
MACHINES	machines[N];
int			costs[N][N];
int			distances[N];
int			myid;
int			sock;
struct sockaddr_in addr;
struct sockaddr_in otheraddr;
socklen_t addr_size;
pthread_mutex_t	lock;


// functions
void *run_link_state (void *);
void *receive_info (void *);
void print_costs (void);


//main()
int main(int argc, char *argv[]){
	if (argc != 4) {
		fprintf(stderr, "usage: %s <myid> <costs> <machines>\n", argv[0]);
		exit(1);
	}
	FILE	*fpc,*fp;
	int		i, j;
	pthread_t	thr1, thr2;
	int		id, cost;
	int		packet[3];
  myid=atoi(argv[1]);
	// get costs
	if ((fpc = fopen (argv[2], "r")) == NULL){
		printf("can't open %s\n", argv[2]);
		return 1;
	}
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++)	{
			fscanf(fpc, "%d", &costs[i][j]);
		}
	}
	fclose (fpc);
	//get info on machines
	if ((fp = fopen(argv[3], "r")) == NULL){
		printf("can't open %s\n", argv[3]);
		return 1;
	}
	for (i = 0; i < N; i++){
	  fscanf(fp, "%s%s%d", machines[i].name, machines[i].ip, &machines[i].port);
  }
	fclose(fp);
    // init address
    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)machines[myid].port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset((char *)addr.sin_zero, '\0', sizeof(addr.sin_zero));
    addr_size = sizeof(addr);
    // create socket
    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf ("socket error\n");
        return 1;
    }

    // bind
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr))!= 0){
        perror("bind error\n");
        return 1;
    }

	// create threads
	pthread_mutex_init (&lock, NULL);
	pthread_create (&thr1, NULL, receive_info, NULL);
	pthread_create (&thr2, NULL, run_link_state, NULL);

	// read changes from the keyboard
	for (i = 0; i < 3; i++)
	{
		printf ("any changes? ");
		scanf ("%d %d", &id, &cost);
		if (id >= N  ||  id == myid) //if the node is out of range or is itself, don't update
		{
			printf ("wrong id\n");
			break;
		}
		pthread_mutex_lock (&lock);//make sure no one can access costs while editting
		costs[myid][id] = cost; //update costs
		costs[id][myid] = cost;
		pthread_mutex_unlock (&lock);
		print_costs();
		packet[0] = htonl(myid);
		packet[1] = htonl(id);
		packet[2] = htonl(cost);
		otheraddr.sin_family = AF_INET;
		addr_size = sizeof(otheraddr);

		for (j = 0; j < N; j++){
			if (j != myid)
			{
				otheraddr.sin_port = htons((short)machines[j].port);
				inet_pton (AF_INET, machines[j].ip, &otheraddr.sin_addr.s_addr);
				sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&otheraddr, addr_size);//send data to update the other nodes
			}
		}
		printf("sent\n");
		print_costs();
	}
	sleep (20);

}


// receive info
void * receive_info (void *arg){
  int rec[3];
  int bytes;
  int newCost1, newCost2, newCost3;
  while(1){
    if(recvfrom(sock, rec, sizeof(int)*3, 0, NULL, NULL)!=12){
      printf("Error: can't receive\n");
      exit(1);
    }
    newCost1 = ntohl(rec[0]);
    newCost2 = ntohl(rec[1]);
    newCost3 = ntohl(rec[2]);
    pthread_mutex_lock(&lock);
    costs[newCost1][newCost2]=newCost3;//update costs based on info recieved
    costs[newCost2][newCost1]=newCost3;
    pthread_mutex_unlock(&lock);
    print_costs();
  }
}


// run_link_state
void * run_link_state (void *arg)
{
	int	taken[N];
	int	min, spot;
	int	i, j, k;
	int	r;

	while (1)
	{
		r = rand () % 10;
		sleep (r);

		for (i = 0; i < N; i++){
			taken[i] = 0;
			pthread_mutex_lock(&lock);
			distances[i] = costs[myid][i];
			pthread_mutex_unlock(&lock);

		}
		taken[myid] = 1;

		for (i = 1; i < N; i++){
      min = INFINITE;
      spot=-1;
      for(k=0; k<N; k++){
          if(distances[k]<min && taken[k]==0){ //find the current minimum distance
            spot=k;
            min=distances[k];
          }
      }
      taken[spot] = 1;//min node
			// recalculate distances
			for (j = 0; j < N; j++)
			{
				if (taken[j] == 0)
				{
					pthread_mutex_lock(&lock);
					distances[j] = MIN(distances[j], (distances[spot] + costs[spot][j]));//see if the newly added costs is the new current lowest cost
					pthread_mutex_unlock(&lock);
				}
			}
			printf("\n");
		}

		printf ("new distances:\n");
		for (i = 0; i < N; i++)
			printf ("Distance to node %d: %d \n", i, distances[i]);
		printf ("\n");
		print_costs();
	}
	printf("out of run link");
	print_costs();
}


// print costs
void print_costs (void){
	int i, k;
  for(i=0; i<N; i++){
    for(k=0; k<N;k++){
      pthread_mutex_lock(&lock);
      printf("%d ", costs[i][k]);
      pthread_mutex_unlock(&lock);
    }
    printf("\n");
  }
}
