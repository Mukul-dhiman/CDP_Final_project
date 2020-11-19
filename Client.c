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

void remove_end_character(char* string){
  int len = strlen(string);
  // len-1 is the last character before the original \0
  if(len > 0 && string[len-1] == '\n'){
    // remove the new-line by ending the string sooner
    string[len-1] = '\0';
  }
  return;
}

char *getreq(char *inbuf, int len) {
  /* Get request char stream */
  printf("REQ: ");              /* prompt */
  memset(inbuf,0,len);          /* clear for good measure */
  return fgets(inbuf,len,stdin); /* read up to a EOL */
}

char *get_name(char *inbuf, int len) {
  /* Get request char stream */
  printf("Enter Your Name: ");              /* prompt */
  memset(inbuf,0,len);          /* clear for good measure */
  return fgets(inbuf,len,stdin); /* read up to a EOL */
}

char *get_option(char *inbuf, int len) {
  /* Get request char stream */
  printf("press I for individual mode\n");              /* prompt */
  printf("press G for Group mode\n"); 
  printf("press A for Admin mode\n"); 
  memset(inbuf,0,len);          /* clear for good measure */
  return fgets(inbuf,len,stdin); /* read up to a EOL */
}


int n;
char sndbuf[MAXIN], rcvbuf[MAXOUT],name[MAXIN];
void from_main_menu(int sockfd){
  char option[MAXIN];
  get_option(option,MAXIN);
  while (!strcmp(option,"A") && !strcmp(option,"G") && !strcmp(option,"I")){
    printf("Enter valid option\n");
    get_option(option,MAXIN);
  }
  
  n=write(sockfd, option, strlen(option)); /* send */
  if(n<0){
     error("Error on selection mode");
  }
    
  memset(rcvbuf,0,MAXOUT);               /* clear */
  n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
  if(n<0){
    error("Error in read");
  }
  remove_end_character(option);
  printf("%s",option);
  if(option[0]=='A'){
    //Admin
    printf("Now you are in Admin mode");





  }
  else if(option[0]=='I'){
    //Individual
    printf("Now you are in Individual Quiz mode");





  }
  else{
    //Group
    printf("Now you are in Group Quiz mode");



    

  }
  


  // while(1){
  //   getreq(sndbuf, MAXIN);
  //   if(sndbuf=="q"){
  //     break;
  //   }
  //   n=write(sockfd, sndbuf, strlen(sndbuf)); /* send */
  //   if(n<0){
  //     error("Error on Write");
  //   }
  //   printf("Wrote message: %s\n",sndbuf);
    
  //   memset(rcvbuf,0,MAXOUT);               /* clear */
  //   n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
    
  //   if(n<0){
  //     error("Error in read");
  //   }

  //   printf("Received reply: %d\n",n);
    
  // }
}

void client(int sockfd) {
  get_name(name,MAXIN);
  n=write(sockfd, name, strlen(name)); /* send */
  if(n<0){
     error("Error on Write name");
  }
    
  memset(rcvbuf,0,MAXOUT);               /* clear */
  n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
  if(n<0){
    error("Error in read");
  }

  if(rcvbuf){
    printf("**************************************\n");
    printf("*                                    *\n");
    printf("   Welcome %s to online Quiz on OS \n",rcvbuf);
    printf("*                                    *\n");
    printf("**************************************\n");
  }

  from_main_menu(sockfd);
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
