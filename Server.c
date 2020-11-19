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

void remove_end_character(char* string){
  int len = strlen(string);
  // len-1 is the last character before the original \0
  if(len > 0 && string[len-1] == '\n'){
    // remove the new-line by ending the string sooner
    string[len-1] = '\0';
  }
  return;
}

//socket for servers
int lstnsockfd,current_number_of_users,n; 
void main_menu(int consockfd,char* name){
  char option[MAXREQ];
  memset(option,0, MAXREQ);
  n = read(consockfd,option,MAXREQ-1); /* Recv */
  remove_end_character(option);
  printf("user %s Selected %s mode\n", name,option);
  if (n <= 0) return;
  n = write(consockfd, option, strlen(option)); /* echo*/
  printf("%s",option);
  if(option[0]=='A'){
    //Admin



  }
  else if(option[0]=='I'){
    //Individual



  }
  else{
    //Group
    

    

  }
}

void server(int consockfd) {
  char reqbuf[MAXREQ],name[MAXREQ];

  memset(name,0, MAXREQ);
  n = read(consockfd,name,MAXREQ-1); /* Recv */
  remove_end_character(name);
  printf("user name registored as:%s\n", name);
  if (n <= 0) return;
  n = write(consockfd, name, strlen(name)); /* echo*/
  
  main_menu(consockfd,name);

  while (1) {                   
    memset(reqbuf,0, MAXREQ);
    n = read(consockfd,reqbuf,MAXREQ-1); /* Recv */
    
    printf("Recvd msg:%s\n", reqbuf);
    if (n <= 0) return;
    n = write(consockfd, reqbuf, strlen(reqbuf)); /* echo*/
  }
}


struct info{
	int val;
};

static void* go(void* arg){
	int thread_id;
	struct info *temp = arg;
	thread_id=temp->val;

	printf("Hello from thread %d\n" , thread_id );

  int consockfd, clilen;
  struct sockaddr_in cli_addr;
  while(1){
    printf("Listening for incoming connections\n");

    /* Listen for incoming connections */
    listen(lstnsockfd, MAXQUEUE); 

  //  clilen = sizeof(cl_addr);

  /* Accept incoming connection, obtaining a new socket for it */
    consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,&clilen);

    if(consockfd<0){
      error("Error on Accepting");
    }
    current_number_of_users++;
    printf("Accepted connection, current user %d\n",current_number_of_users);

    server(consockfd);

    close(consockfd);
    current_number_of_users--;
  }
  
	pthread_exit(&(thread_id)+100);  //(int*)(100 + n)
	return NULL;
	// Not reached
}


static pthread_t threads[MAXQUEUE];

int main(int argc,char **argv){
  current_number_of_users=0;
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

  //Passing struct for Thread id
  struct info thread_id_struct[MAXQUEUE];
  int ii;
  for( ii = 0; ii < MAXQUEUE ; ii++){
    thread_id_struct[ii].val=ii;
  }
  //Threads for queueing MAXQUEUE number of clients
  for( ii = 0; ii < MAXQUEUE ; ii++){
	  pthread_create(&(threads[ii]),NULL,&go,&thread_id_struct[ii]);
  }
  long ret;
  for ( ii = 0; ii < MAXQUEUE ; ii++){
	  ret = pthread_join(threads[ii],(void**)&ret);
	  printf("Thread %d returned %ld\n" , ii , ret );
  }
  printf( "Main thread done.\n" );


  close(lstnsockfd);
}
