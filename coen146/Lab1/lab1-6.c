//Name: Hanna Albright
//Date: 04/01/21
//Title: Lab1 -Unix/Linux Commands and Overview of C Programming
//Description: This file includes the code for step 6 and step 7 and has been labelled accordingly. The circuitUsers program (step 6) uses the link and user bandwidth to calculate the number of circuit switch users. The packetswitches program (step 7) takes in the percentage of time a packet switching user is transmitting and the number of packet switching users. It also has two helper functions which help calculate the co-efficients of binomial distribution and solve the various subproblems (each which finds the probability of different packet switching situations).
#include <stdio.h>      /* printf, stderr */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork */
#include <stdlib.h>     /* atoi */
#include <errno.h>      /* errno */ 
#include <math.h>

int circuitUsers(int linkBandwidth, int userBandwidth){//step 6
	int nCSusers=linkBandwidth/userBandwidth; //calculate number of circuit switching users
	return nCSusers;
}
double bidistCoefficient(int users, int morethan);
float factorial(float num);

void packetswitches(double tPSuser, int nPSusers){//step 7
	double pPSusersBusy=tPSuser;//7a
	printf("The probability that a given (specific) user is busy transmitting: %f \n ", pPSusersBusy);
	double pPSusersNotBusy=1.0-pPSusersBusy;//7b 
	printf("The probability that one specific user is not busy: %f \n ", pPSusersNotBusy);
	double exp=((double)nPSusers)-1.0; //exponent for step c
	double allUsersNotBusy;
	allUsersNotBusy=pow(pPSusersNotBusy,exp);//7c
	printf("The probability that all of the other specific users are not busy: %f \n ", allUsersNotBusy );
	double oneBusy=pPSusersBusy*allUsersNotBusy;//7d
	printf("The probability that one specific user is transmitting and the remaining users are not transmitting: %f \n ", oneBusy);
	double onePSuserBusy=nPSusers*oneBusy;//7e
	printf("The probability that exactly one of thenPSusersusers is busy: %f \n ", onePSuserBusy);
	double exp2=nPSusers-10;
	double tenPSusersBusy=(pow(pPSusersBusy,10))*(pow(pPSusersNotBusy,exp2));//7f
	printf("The probability that 10 specific users of nPSusers are transmitting and the others are idle: %g \n ", tenPSusersBusy);
	double anyTenBusy=(bidistCoefficient(nPSusers,10))*tenPSusersBusy;//7e
	printf("The probability that any 10 users of nPSusers are transmitting and the others are idle: %g \n ", anyTenBusy);
	int i;
	double step7h=0;
	for (i=11;i<=nPSusers;i++){ //summation for step h calculation
		float coeff=bidistCoefficient(nPSusers, i);
		double exp3=nPSusers-((double)i);
		step7h=step7h+(coeff*pow(pPSusersBusy,((double)i))*pow(pPSusersNotBusy,exp3));
	}
	printf("The probability that more than 10 users of nPSusers are transmitting and the others are idle: %g \n ", step7h);
}

double bidistCoefficient(int users, int morethan){//calculates the binomial distribution using the number of users and probability
	float numerator=factorial(users); //uses factorial function
	users=users-morethan; 
	float denominator=(factorial(users)*factorial(morethan));
	return ((float)(numerator/denominator));
	
}
float factorial(float num){ //function which compute the factorial of a number
	if (num==0)
		return 1;
	return num*factorial(num-1);
}

int main(int argc, char *argv[]){
	int lBandwidth=atoi(argv[1]); //take inputs from the terminal
	int uBandwidth=atoi(argv[2]);
	double tPS=atof(argv[3]);
	int PSusers=atoi(argv[4]);
	int CSusers;
	CSusers=circuitUsers(lBandwidth, uBandwidth);
	printf("Circuit switching scenario creates %d circuit switching users. \n", CSusers);
	packetswitches(tPS, PSusers);
	return 0;	
}
