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

 	///////////////////////////////////////////////////////////////////////////////
 	///PROJECT 5 STUFFF

 		//PROJECT 5 STUFF

	//FD POLLIN STUFF
	int nfds = 64;
	int time = 100;
	struct pollfd pfds[64];
//	pfds[0].fd = mySocket;
//	pfds[0].events = POLLIN;

//Need fds for stdin/TCP/UDP

	//STDIN_FILENO
	//UDPFD
	//TCPFD

	

	int amount = poll(pfds, nfds, time);
	if(pfds[nfds].events == POLLIN){
		
	}
	
	int acceptValue = 0;
	int currentSize = nfds;
	for(int i = 0; i < currentSize; i++){
		if(pfds[i].revents == 0){
			continue;
		}

		if(pfds[i].revents != POLLIN){
			printf("Error");
		}
		if(pfds[i].fd == mySocket){
			//accept(socket, sockaddr * --Should be either null or points to a sockaddr struct where the address should be retuned, length of address)
			while(acceptValue != -1){
				acceptValue = accept(mySocket, NULL, NULL);
				if(acceptValue == -1){
					perror("ERROR");
					exit(EXIT_FAILURE);
				}
			}
		}
	}

 	//UDP Socket Creation
	struct addrinfo hintsUDP;
	struct addrinfo *resultUDP, *rpUDP;
	int getaddrinfoValueUDP;
 	
 	memset(&hintsUDP, 0, sizeof(hintsUDP));
    hintsUDP.ai_family = PF_INET;    // Allow IPv4 or IPv6
    hintsUDP.ai_socktype = SOCK_DGRAM; // Datagram socket
    hintsUDP.ai_flags = AI_PASSIVE;    // Any IP address (DHCP)
    hintsUDP.ai_protocol = IPPROTO_UDP;          // Any protocol
    hintsUDP.ai_canonname = NULL;
    hintsUDP.ai_addr = NULL;
    hintsUDP.ai_next = NULL;

    getaddrinfoValueUDP = getaddrinfo(NULL, "8221", &hintsUDP, &resultUDP);
    if (getaddrinfoValueUDP != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfoValueUDP));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
      Try each address until we successfully bind().
      If socket() or bind() fails, we close the socket
      and try the next address. 
    */

    for (rpUDP = resultUDP; rpUDP != NULL; rpUDP = rpUDP->ai_next) {
        int udpSocket = socket(rpUDP->ai_family, rpUDP->ai_socktype, rpUDP->ai_protocol);
        if (udpSocket == -1)
            continue;

        if (bind(udpSocket, rpUDP->ai_addr, rpUDP->ai_addrlen) == 0)
        	if(setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue)) == 0){
        		for(int i = 0; i <= 64; i++){
        			if(pfds[i].revents & POLLIN){
        				if(pfds[i].fd == udpSocket)
        					recvfrom(udpSocket);
        			}
        		}
        	}
            break;  // Success

        close(udpSocket);
    }

    freeaddrinfo(resultUDP);  // No longer needed

    if (rpUDP == NULL) {  // No address succeeded
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }




//////////////LAB 7 CODE!!/////////////////////////////////////////////////////////////
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


	int s = sendto(mySocket, (const char *)message, strlen(message), 0, (const struct sockaddr *) &mySockAddr, length);
	int s2 = sendto(mySocket, (const char *)message2, strlen(message2), 0, (const struct sockaddr *) &mySockAddr, length);

	if(s == -1 || s2 == -1){
		perror("Could not successfully send message");
		exit(EXIT_FAILURE);
	}



	


	//TCP SOCKET STUFF
	

	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int tcpSocket;
	    
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET;    // Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE;    // Any IP address (DHCP)
	hints.ai_protocol = IPPROTO_TCP;          // Any protocol
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	
	int getaddrinfoValueTCP = getaddrinfo(NULL, "8221", &hints, &result);
	if (getaddrinfoValueTCP != 0) {
	    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfoValueTCP));
	    exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
	        int tcpSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
	        if (tcpSocket == -1)
	            continue;
	
	        if (bind(tcpSocket, rp->ai_addr, rp->ai_addrlen) == 0) 
	        	if (listen(mySocket, 64) == 0)
	        		if(setsockopt(tcpSocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue)) == 0)
	        			if(ioctl(tcpSocket, FIONBIO, &optionValue) == 0){
	        				for(int i = 0; i <= 64; i++){
	        					if(!pfds[i].fd){
	        						pfds[i].fd = tcpSocket;
	        						pfds[i].events = POLLIN;
	        					}
	        				}
	        				break;
	        			}
	        			
	        	

	        close(tcpSocket);
	}
	
	freeaddrinfo(result);  // No longer needed
	
	 if (rp == NULL) {  // No address succeeded
	    fprintf(stderr, "Could not bind\n");
	    exit(EXIT_FAILURE);
	 }


	 pfds[0].fd = STDIN_FILENO;
	 pfds[0].events = POLLIN;
	 nfds++;

	 pfds[1].fd = udpSocketFunction();
	 pfds[1].events = POLLIN;
	 nfds++;

	 pfds[2].fd = tcpSocket;
	 pfds[2].evetns = POLLIN;
	 nfds++;


	    
	
	
//	printf("SEND TO: %d \nMESSAGE LENGTH: %zu \n", s, strlen(message) );
	printf("%s\n", message);
	
	sleep(2);
	
//	printf("SEND TO: %d \nMESSAGE LENGTH: %zu \n", s2, strlen(message) );
	printf("%s\n", message2);

	retur_FILn 0;

	
	
}
