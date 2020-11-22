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
#include <stdbool.h> 

#define MAXIN 20
#define MAXOUT 20

void Individual_mode(int sockfd);
void from_main_menu(int sockfd);


int n;
char sndbuf[MAXIN], rcvbuf[MAXOUT],name[MAXIN];


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



char *get_topic_input(char *Request,int len){
  memset(Request,0,len);          /* clear for good measure */
  return fgets(Request,len,stdin); /* read up to a EOL */
}


char *get_next_request(char *next_request,int len){
  printf("To attempt another question press 'n';\n");
  printf("press 'q' to quit;\n");
  printf("press 'r' to return to main menu\n");
  memset(next_request,0,len);          /* clear for good measure */
  return fgets(next_request,len,stdin); /* read up to a EOL */

}

char *get_answer(char *answer,int len){
  printf("give your answer in options:");  
  memset(answer,0,len);          /* clear for good measure */
  return fgets(answer,len,stdin); /* read up to a EOL */

}

void show_all_topics(char* all_topics){
  // showing all topics
  printf("%s",all_topics);

  return;
}

void question(char* question,int sockfd){
  // represent question
  printf("%s\n",question);

  // get answer
  char answer[MAXIN];
  bool is_answer_correct=true;
  get_answer(answer,MAXIN);
  // check anwer is correct or not





  if(is_answer_correct){
    printf("correct ,");
  }
  else{
    printf("wrong ,");
  }
  printf("Here is the explanation\n");
  // print explanation



  

  char next_request[MAXIN];
  get_next_request(next_request,MAXIN);
  n=write(sockfd, next_request, strlen(next_request)); /* send */
  if(n<0){
    error("Error on requesting topics");
  }
  
  memset(rcvbuf,0,MAXOUT);               /* clear */
  n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
  if(n<0){
    error("Error in read topics");
  }
  remove_end_character(next_request);
  if(strcmp(next_request,"n")==0){
    printf("'n' pressed\n");
    Individual_mode(sockfd);
  }
  else if(strcmp(next_request,"r")==0){
    printf("'r' pressed\n");
    from_main_menu(sockfd);
  }
  else if(strcmp(next_request,"q")==0){
    printf("'q' pressed\n");
    return;
  }
  else{
    printf("invalid key pressed\n");
  }
  return;
}


void Individual_mode(int sockfd){
  //Individual
  char Request_topics[]="give_topics",Request[MAXIN];
  n=write(sockfd, Request_topics, strlen(Request_topics)); /* send */
  if(n<0){
    error("Error on requesting topics");
  }   
  memset(rcvbuf,0,MAXOUT);               /* clear */
  n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
  if(n<0){
    error("Error in read topics");
  }

  // this print statement will show all the topics listed
  show_all_topics(rcvbuf);
    
  printf("\n");
  //assuming client input valid topic 
  get_topic_input(Request,MAXIN);

  n=write(sockfd, Request, strlen(Request)); /* send */
  if(n<0){
    error("Error on requesting specific topics");
  }
    

  memset(rcvbuf,0,MAXOUT);               /* clear */
  n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
  if(n<0){
    error("Error in read topics");
  }
  question(rcvbuf,sockfd);
}

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
  if(option[0]=='A'){
    //Admin
    printf("Now you are in Admin mode");
    




  }
  else if(option[0]=='I'){
    printf("Now you are in Individual Quiz mode\n");
    Individual_mode(sockfd);

  }
  else{
    //Group
    printf("Now you are in Group Quiz mode\n");





  }
  
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
  return;
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
  printf("closing session\n");
	close(sockfd);
  return 0;
}
