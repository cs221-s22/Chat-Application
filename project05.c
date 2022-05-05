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



//////////////////////TCP SOCKET CREATION/////////////////////////////////////////////////
int tcpSocketFunction(char * port){
	int tcpSocket;


	struct addrinfo hints;
	struct addrinfo *result, *rp;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;    // Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_protocol = IPPROTO_TCP; 
	hints.ai_flags = AI_PASSIVE;    // Any IP address (DHCP)
	hints.ai_protocol = 0;          // Any protocol
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	

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
	}
	freeaddrinfo(result);  // No longer needed
		
	if (rp == NULL) {  // No address succeeded
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}
	return tcpSocket;
	
}

/////TCP MESSENGER//////////////////
int tcpMessenger(char* port){
struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res;
    int rc = getaddrinfo("HOSTNAME", port, &hints, &res);
    if (rc != 0)
        printf("%s\n", gai_strerror(rc));
    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (connect(fd, res->ai_addr, res->ai_addrlen) != 0)
        perror("connect");

    char *msg = "test";
    int len = strlen(msg) + 1;
    if (send(fd, msg, len, 0) == -1)
        perror("send");

    if (close(fd) == -1)
        perror("close");

    return 0;
  }

//ONLINE PRESENCE FUNCTION///////////////////////////////////////////

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

	
	
	strncat(message, user, strlen(user));


	strcat(message, " ");

	strncat(message, port, strlen(port));
	
	strncat(message, "\0", 1);


	int s = sendto(udpSocket, (const char *)message, strlen(message), 0, (const struct sockaddr *) &mySockAddr, length);

	
	if(s == -1){
		perror("Could not successfully send message");
		exit(EXIT_FAILURE);
	}
	
}


////////////////////OFFLINE PRESENCE////////////////////
void  offlinePresence(int udpSocket, char* user, char* port){
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


	char message2[50] = "offline: ";
	
	

	strncat(message2, user, strlen(user));

	strcat(message2, " ");

	strncat(message2, port, strlen(port));
	

	strncat(message2,"\0", 1);

	int s2 = sendto(udpSocket, (const char *)message2, strlen(message2), 0, (const struct sockaddr *) &mySockAddr, length);
	
	if(s2 == -1){
		perror("Could not successfully send message");
		exit(EXIT_FAILURE);
	}
	
}

//UDP SOCKET CREATION/////////////////////////////////////

int udpSocketFunction(){
	int mySocket;
	int optionValue = 1;
	
	mySocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(mySocket == -1){
		perror("Could not create socket");
		exit(EXIT_FAILURE);
	}
	
	int socketOpt1 = setsockopt(mySocket, SOL_SOCKET, SO_BROADCAST, &optionValue, sizeof(optionValue));
	if(socketOpt1 == -1){
		int socketOpt2 = setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
		if(socketOpt2 == -1){
			int socketOpt3 = setsockopt(mySocket, SOL_SOCKET, SO_REUSEPORT, &optionValue, sizeof(optionValue));
			if(socketOpt3 == -1){
						perror("could not establish socket options!");
						exit(EXIT_FAILURE);
					}
		}
	}

	return mySocket;
}




int main(int argc, char *argv[]){
	int optionValue = 1;

	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len;
	ssize_t nread;
	char buf[BUF_SIZE];
	int totalUsers = 0;


	//////////USER INFORMATION STRUCT///////////////////////////////////////////////////
	struct userInfo{
		char presence[BUF_SIZE];
		char userName[BUF_SIZE];
		char portNumber[BUF_SIZE];
		char userHost[BUF_SIZE];
	};
	
	struct userInfo hosts[64];
	

//SERVER STUFF/////////////////////////////////////////
	memset(&hints, 0, sizeof(hints));
  	hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
   	hints.ai_socktype = SOCK_DGRAM; // Datagram socket
    hints.ai_flags = AI_PASSIVE;    // Any IP address (DHCP)
    hints.ai_protocol = 0;          // Any protocol
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(NULL, "8221", &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
            sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            int socketOpt1 = setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, &optionValue, sizeof(optionValue));
            //	if(socketOpt1 == -1){
            int socketOpt2 = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
            int socketOpt3 = setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &optionValue, sizeof(optionValue));
            
            if (sfd == -1)
                continue;
    
            if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
                break;  // Success
   
        }
    
        freeaddrinfo(result);  // No longer needed
    
        if (rp == NULL) {  // No address succeeded
            fprintf(stderr, "Could not bind\n");
            exit(EXIT_FAILURE);
        }
    
//POLLIN STUFF/////////////////////////////////////////////////////////
	int amount;

	char* user = argv[1];
	char* port = argv[2];

	
	//TCP AND UDP SOCKETS BEING MADE
	int myUdp = udpSocketFunction();
	int myTcp = tcpSocketFunction(port);

	int nfds = 0;
	int time = 100;
	struct pollfd pfds[64];

	//SETTING FDS and POLLINs

	pfds[0].fd = STDIN_FILENO;
	pfds[0].events = POLLIN;
	nfds++;

	pfds[1].fd = sfd;
	pfds[1].events = POLLIN;
	nfds++;

	pfds[2].fd = myTcp;
	pfds[2].events = POLLIN;
	nfds++;


//WHILE LOOP TO RUN INFINITELY///////////////////
	bool end_loop = false;
	
	presence(pfds[1].fd, user, port);
	int presenceCountdown = 175;
	while(!end_loop){
		presenceCountdown--;
	//	printf("COUNTDOWN NUMBER: %d\n", presenceCountdown );
		if(presenceCountdown == 0){
			presence(pfds[1].fd, user, port);
			presenceCountdown = 175;
		}
		amount = poll(pfds, nfds, time);		
		for(int i = 0; i <= 64; i++){
			if(pfds[i].revents & POLLIN){
			//STDIN FILENO FD
				if(pfds[i].fd == STDIN_FILENO){
					printf("IN STDIN CHECK\n");
					char stdinBuf[50] = "";
					char ch = ' ';
					ch = getchar();
					if(ch == EOF){
						printf("OFFLINE\n");
						offlinePresence(pfds[1].fd, user, port);
						exit(EXIT_SUCCESS);
					}
					while(ch != '\n'){
						ch = getchar();
						strncat(stdinBuf, &ch, 1);
					}
					printf("%s\n", stdinBuf);
				}
				//PRESENCE FD
				else if(pfds[i].fd == sfd){
					peer_addr_len = sizeof(peer_addr);
					nread = recvfrom(sfd, buf, BUF_SIZE, 0,
					(struct sockaddr *) &peer_addr, &peer_addr_len);
					if (nread == -1)
					   continue;  // Ignore failed request 
					
					char host[NI_MAXHOST], service[NI_MAXSERV];
					
					s = getnameinfo((struct sockaddr *) &peer_addr,peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
					if (s == 0){
					   char nullTerm = '\0';
					   buf[nread] = nullTerm;
					   printf("Presence: %s on host host: %s%c\n", buf, host, nullTerm);

					   //POPULATE THE USER STRUCT HERE
					   char* userPresence = strtok(buf, " ");
					   char* userName = strtok(NULL, " ");
					   char* userPort = strtok(NULL, " ");

					   int flag = 0;
						
			
					   int count = 0;
					   while(count < 64){
					   		count++;
					   		if(!strcmp(hosts[count].portNumber, userPort)){
					   			flag = 1;
					   			break;
					   		}
					   }
// 
					   if(flag == 0){
					   	strcpy(hosts[totalUsers].presence, userPresence);
					   	strcpy(hosts[totalUsers].userName, userName);
					   	strcpy(hosts[totalUsers].portNumber, userPort);
					   	strcpy(hosts[totalUsers].userHost, host);
					   	totalUsers++;
					   }
  
					 }
					else{
					   fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
					   }
					   
				}
				else if(pfds[i].fd == myTcp){
					int acceptValue = accept(myTcp, NULL, NULL);
					read(acceptValue, buf, sizeof(buf));

					/////////////////////
				//	tcpMessenger(port);
					//////
					
				}
			}
		}  
		 
	}
		
	}

		
	

	
	  
	
