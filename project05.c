#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <stdbool.h>
#define BUF_SIZE 500


struct userInfo{
	char* userName;
	char* portNumber;
	char* port;
	
};

struct userInfo hosts[64];


int tcpSocketFunction(char* port){
	int tcpSocket;


	struct addrinfo hints;
	struct addrinfo *result, *rp;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;    // Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_protocol = IPPROTO_TCP; 
	

//	tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int getaddrinfoValueTCP = getaddrinfo(NULL, port, &hints, &result);
	if (getaddrinfoValueTCP != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfoValueTCP));
		exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		 tcpSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (tcpSocket == -1)
		   continue;

		int optionValue = 1;

		if (bind(tcpSocket, rp->ai_addr, rp->ai_addrlen) == 0)
			if (listen(tcpSocket, 64) == 0)
		   		 if(setsockopt(tcpSocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue)) == 0)
		      		  if(ioctl(tcpSocket, FIONBIO, &optionValue) == 0){
		      		  	break;
		      		  }
	//	close(tcpSocket);

	}

	freeaddrinfo(result);  // No longer needed
		
	if (rp == NULL) {  // No address succeeded
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}
	
}

void  presence(int udpSocket, char* user, char* port){
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

	char message[50] = "online: ";
//	char message2[50] = "offline: ";
	
	
	strncat(message, user, strlen(user));
//	strncat(message2, user, strlen(user));

	strcat(message, " ");

	strncat(message, port, strlen(port));
	
	strncat(message, "\0", 1);
//	strncat(message2,"\0", 1);

	int s = sendto(udpSocket, (const char *)message, strlen(message), 0, (const struct sockaddr *) &mySockAddr, length);
//	int s2 = sendto(mySocket, (const char *)message2, strlen(message2), 0, (const struct sockaddr *) &mySockAddr, length);
	
	if(s == -1){
		perror("Could not successfully send message");
		exit(EXIT_FAILURE);
	}
	
}

int udpSocketFunction(){
	int mySocket;
	int optionValue = 1;
	
	mySocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(mySocket == -1){
		perror("Could not create socket");
		exit(EXIT_FAILURE);
	}
	
	int socketOpt1 = setsockopt(mySocket, SOL_SOCKET, SO_BROADCAST, &optionValue, sizeof(optionValue));
//	if(socketOpt1 == -1){
	int socketOpt2 = setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
	int socketOpt3 = setsockopt(mySocket, SOL_SOCKET, SO_REUSEPORT, &optionValue, sizeof(optionValue));
	//	if(socketOpt2 == -1){
	//		perror("could not establish socket options!");
	//		exit(EXIT_FAILURE);
	//	}
			 
//	}
//	presence(mySocket, user, port);

	return mySocket;
}




int main(int argc, char *argv[]){

	// for(int i = 0; i <= 64; i++){
		// if(hosts[i].userName == NULL){
			// hosts[i].userName = argv[1];
			// hosts[i].portNumber = argv[2];
		// }
	// }



	char* user = argv[1];
	char* port = argv[2];
	char buffer[128];
//	udpSocketFunction();
// //	bool end_loop = false;
	int amount;

	int nfds = 0;
	int time = 100;
	struct pollfd pfds[64];

	pfds[0].fd = STDIN_FILENO;
	pfds[0].events = POLLIN;
	nfds++;

	int myUdp = udpSocketFunction();
	int myTcp = tcpSocketFunction(port);
	 
	pfds[1].fd = myUdp;
	pfds[1].events = POLLIN;
	nfds++;

	
	bool end_loop = false;
	presence(pfds[1].fd, user, port);
	
	
	while(!end_loop){
		bool flag = false;
		
		amount = poll(pfds, nfds, time);		
		for(int i = 0; i <= 64; i++){
		
			if(pfds[i].revents & POLLIN){
			
				if(pfds[i].fd == STDIN_FILENO){
					char ch = ' ';
					while(ch != '\n'){
						ch = getchar();
						printf("%c", ch);
						if(ch == EOF){
							printf("EOF\n");
						//	break;
							exit(EXIT_SUCCESS);
						}
					}
					
				}
				
				else if(pfds[i].fd == myUdp){
					printf("UDP STUFF\n");
					pfds[i].events = POLLHUP;
				}
				
				 else if(pfds[i].fd == myTcp){
					 pfds[i].events = POLLHUP;
					 printf("TCP STUFF");
			 	}

				else{
					pfds[i].events = POLLHUP;
				}
				
				flag = true;
			}

			if(flag){
				continue;
			}

			else if(amount == 0){
				for(int i = 0; i < 100; i++){
					poll(pfds, nfds, 100);
				}
				presence(pfds[1].fd, user, port);
			}
		}
			
		}
		
	}

		
		
		
		// if(pfds[0].revents & POLLIN){
			// 
		// }
		// if(pfds[1].revents & POLLIN){
			// 
		// }


	
	  
	
