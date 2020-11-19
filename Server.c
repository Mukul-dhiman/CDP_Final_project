// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#define MAXREQ 20
#define MAXQUEUE 5

void error(const char *msg){
  perror(msg);
  exit(1);
}

struct info{
	int val;
};

//socket for servers
int lstnsockfd; 

void server(int consockfd) {
  char reqbuf[MAXREQ];
  int n;
  while (1) {                   
    memset(reqbuf,0, MAXREQ);
    n = read(consockfd,reqbuf,MAXREQ-1); /* Recv */
    
    printf("Recvd msg:%s\n", reqbuf);
    if (n <= 0) return;
    n = write(consockfd, reqbuf, strlen(reqbuf)); /* echo*/
  }
}

static void* go(void* arg){
	int n;
	struct info *temp = arg;
	n=temp->val;

	printf("Hello from thread %d\n" , n );

  int consockfd, clilen;
  struct sockaddr_in cli_addr;
  while (1) {
   printf("Listening for incoming connections\n");

/* Listen for incoming connections */
   listen(lstnsockfd, MAXQUEUE); 

  //  clilen = sizeof(cl_addr);

/* Accept incoming connection, obtaining a new socket for it */
   consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,       
                      &clilen);

    if(consockfd<0){
      error("Error on Accepting");
    }
   printf("Accepted connection\n");

   server(consockfd);

   close(consockfd);
  }
  
	pthread_exit(&(n)+100);  //(int*)(100 + n)
	return NULL;
	// Not reached
}

static pthread_t threads[MAXQUEUE];

int main(int argc,char **argv){

int portno = 5033;
struct sockaddr_in serv_addr, cli_addr;

 memset((char *) &serv_addr,0, sizeof(serv_addr));
 serv_addr.sin_family      = AF_INET;
 serv_addr.sin_addr.s_addr = INADDR_ANY;
 serv_addr.sin_port        = htons(portno);

// Server protocol
/* Create Socket to receive requests*/
lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);

if(lstnsockfd<0){
  error("Error opening Sockets.");
}

/* Bind socket to port */
if(bind(lstnsockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
  error("binding failed");
}
printf("Bounded to port\n");



//Threads for queueing MAXQUEUE number of clients
int ii;
for ( ii = 0; ii < MAXQUEUE ; ii++){
	struct info *temp;
	temp->val=ii;
	pthread_create(&(threads[ii]),NULL,&go,temp);
}
long ret;
for ( ii = 0; ii < MAXQUEUE ; ii++){
	ret = pthread_join(threads[ii],(void**)&ret);
	printf("Thread %d returned %ld\n" , ii , ret );
}
printf( "Main thread done.\n" );


close(lstnsockfd);
}
