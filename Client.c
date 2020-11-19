// Client.c
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXIN 20
#define MAXOUT 20

void error(const char *msg){
  perror(msg);
  exit(1);
}

char *getreq(char *inbuf, int len) {
  /* Get request char stream */
  printf("REQ: ");              /* prompt */
  memset(inbuf,0,len);          /* clear for good measure */
  return fgets(inbuf,len,stdin); /* read up to a EOL */
}


void client(int sockfd) {
  int n;
  char sndbuf[MAXIN]; char rcvbuf[MAXOUT];
  while(1){
    getreq(sndbuf, MAXIN);
    if(sndbuf=="q"){
      break;
    }
    n=write(sockfd, sndbuf, strlen(sndbuf)); /* send */
    if(n<0){
      error("Error on Write");
    }
    printf("Wrote message: %s\n",sndbuf);
    
    memset(rcvbuf,0,MAXOUT);               /* clear */
    n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
    
    if(n<0){
      error("Error in read");
    }

    printf("Received reply: %d\n",n);
    
  }
}

// Server address
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr,
	char *serverIP, int portno) {
  /* Construct an address for remote server */
  memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  inet_aton(serverIP, &(serv_addr->sin_addr));
  serv_addr->sin_port = htons(portno);
 }


int main(int argc,char **argv){
	//Client protocol
	// char *serverIP = "13.126.97.198";
  char *serverIP = "127.0.0.1";
	int sockfd, portno = 5033;
	struct sockaddr_in serv_addr;
	
	buildServerAddr(&serv_addr, serverIP, portno);

	/* Create a TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd<0){
    error("Error opening socket");
  }

	/* Connect to server on port */
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
    error("Connection Failed");
  }
	printf("Connected to %s:%d\n",serverIP, portno);

	/* Carry out Client-Server protocol */
	client(sockfd);

	/* Clean up on termination */
	close(sockfd);
}
