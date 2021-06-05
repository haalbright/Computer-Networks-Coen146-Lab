//Name: Hanna
//Date: 04/15/21
//Title: Lab 3: Network commands and HTTP 
//Description: This file contain 4 functions which calculate the duration of time from a request depending on various DNS and HTTP delays. The function duration solves part 3a and find the duration of time from clicking a link to receiving the IP address after lookup (not in the cache). Durationb also calculates the duration of time for receiving n objects but using non-persistent http and no parallel TCP connections. Durationc2 also calculated the duration of time for requesting n objects using non-persistent HTTP, but used 2 parallel connections (2 can be swapped with any number). Finally, Durationc calculated the duration of time when requesting n objects using persistent HTTP and 2 parallel connections (2 can be swapped with any number). In the main function, duration is called, then durationb, durationc, and durationc2 all with a parameter of 6.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
int rtt0=3;
int rtt1=20;
int rtt2=26;
int rtthttp=47;


int duration(){//1 object being sent
	int total= rtt0+rtt1+rtt2+(2*rtthttp); //IP address lookup+ request&receive object 
return total;
}
int durationb(int n){//n=number of objects
        int total= rtt0+rtt1+rtt2+(2*rtthttp)+(n*2*rtthttp); //time with non-persistent HTTP and no parallel TCP connections 
return total;
}

int durationc(int n){ //n objects
        int parallel =n/2;//calculate objects per connection
	if (n%2 > 0){//there are 2 parallel connections (can switch 2 to any number)
		parallel+=1;
	}
	int total= rtt0+rtt1+rtt2+(2*rtthttp)+(parallel*2*rtthttp);//calculate duration using persistent HTTP
return total;
}

int durationc2(int n){//n objects
        int parallel =n/2;//calculate objects per connection
        if (n%2 > 0){ //there are 2 parallel connections (can switch 2 to any number)
                parallel+=1;
        }
        int total= rtt0+rtt1+rtt2+(2*rtthttp)+(parallel*rtthttp);//calculate duration using non-persistent HTTP
return total;
}

int main(){
	printf("Step 3 part a: %d\n",duration());
	printf("Step 3 part b: %d\n",durationb(6));//calculate for 6 objects	
	printf("Step 3 part c: %d\n",durationc(6));	
	printf("Step 3 part c2: %d\n",durationc2(6));
return 0;	
}
