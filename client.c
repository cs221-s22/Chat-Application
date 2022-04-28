#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#define BUF_SIZE 500

int main(int argc, char *argv[]){

	if(argc < 2){
		printf("Usage: Must include port message...");
	}

	int mySocket;
	int optionValue = 1;
	
	struct sockaddr_in mySockAddr;
	
	mySockAddr.sin_family = AF_INET;
	mySockAddr.sin_port = htons(8221);
	inet_pton(PF_INET, "10.10.13.255", &mySockAddr.sin_addr);

	int ptonValue = inet_pton(PF_INET, "10.10.13.255", &mySockAddr.sin_addr);
	if(ptonValue == 0){
		perror("src is not a valid string");
		exit(EXIT_FAILURE);	
	}
		
	if(ptonValue == -1){
		perror("AF is unknown");
		exit(EXIT_FAILURE);
	}

	
	int length = sizeof(mySockAddr);
	//
	
 	char* user = argv[1];


 	char message[50] = "online: ";
 	char message2[50] = "offline: ";


 	strncat(message, user, strlen(user));
 	strncat(message2, user, strlen(user));

 	strncat(message, "\0", 1);
 	strncat(message2,"\0", 1);
 	




	mySocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(mySocket == -1){
		perror("Could not create socket");
		exit(EXIT_FAILURE);
	}

	int socketOpt1 = setsockopt(mySocket, SOL_SOCKET, SO_BROADCAST, &optionValue, sizeof(optionValue));
	if(socketOpt1 == -1){
		 int socketOpt2 = setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));{
		 	if(socketOpt2 == -1){
		 		perror("could not establish socket options!");
		 		exit(EXIT_FAILURE);
		 	}
		 }
	}
//	int socketOpt2 = setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));

	int s = sendto(mySocket, (const char *)message, strlen(message), 0, (const struct sockaddr *) &mySockAddr, length);
	
	int s2 = sendto(mySocket, (const char *)message2, strlen(message2), 0, (const struct sockaddr *) &mySockAddr, length);

	if(s == -1 || s2 == -1){
		perror("Could not successfully send message");
		exit(EXIT_FAILURE);
	}
	
//	printf("SEND TO: %d \nMESSAGE LENGTH: %zu \n", s, strlen(message) );
	printf("%s\n", message);
	
	sleep(2);
	
//	printf("SEND TO: %d \nMESSAGE LENGTH: %zu \n", s2, strlen(message) );
	printf("%s\n", message2);

	return 0;

	
	
}
