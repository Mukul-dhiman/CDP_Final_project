// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <mysql/mysql.h>
#define MAXREQ 20
#define MAXQUEUE 5

void main_menu(int consockfd,char* name);
void Individual_mode(int consockfd,char* name);

//socket for servers
int lstnsockfd,current_number_of_users,n; 


// Mysql database
MYSQL *con;

// MySQL query implmenting functions

typedef struct db_entry
{
  char qtopic[50], qtype[50], qtext[1024], ans[256], expln[1024];
} db_entry;

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void select_db(MYSQL *con)
{

  if (mysql_query(con, "use cn_db"))
  {
    finish_with_error(con);
  }

}

void addEntries(MYSQL *con, db_entry db_entries[], size_t len)
{

  char query[10000] = "INSERT INTO entries (topic,qtype,qtext,ans,expln) VALUES ";

  char entry[500] = "('";

  strncat(entry, db_entries[0].qtopic,1000);
  strncat(entry, "','",1000);
  strncat(entry, db_entries[0].qtype,1000);
  strncat(entry, "','",1000);
  strncat(entry, db_entries[0].qtext,1000);
  strncat(entry, "','",1000);
  strncat(entry, db_entries[0].ans,1000);
  strncat(entry, "','",1000);
  strncat(entry, db_entries[0].expln,1000);
  strncat(entry, "')",1000);
  strncat(query, entry,1000);

  for (int i = 1; i < len; i++)
  {
    char entry[] = ",('";
    strncat(entry, db_entries[i].qtopic,1000);
    strncat(entry, "','",1000);
    strncat(entry, db_entries[i].qtype,1000);
    strncat(entry, "','",1000);
    strncat(entry, db_entries[i].qtext,1000);
    strncat(entry, "','",1000);
    strncat(entry, db_entries[i].ans,1000);
    strncat(entry, "','",1000);
    strncat(entry, db_entries[i].expln,1000);
    strncat(entry, "')",1000);
    strncat(query, entry,1000);
  }

  if (mysql_query(con, query))
  {
    finish_with_error(con);
  }

}

void getTopics(MYSQL *con, char *qresult)
{
  char query[10000] = "SELECT DISTINCT topic from entries";
  if (mysql_query(con, query))
  {
    finish_with_error(con);
  }
  MYSQL_RES *result = mysql_store_result(con);
  int num_fields = mysql_num_fields(result);
  MYSQL_ROW row;

  while (row = mysql_fetch_row(result))
  {
    printf("%s - ",row[0]);
    strncat(qresult, row[0],1000);
    strncat(qresult, "\n",1000);
  }
  printf("\n");
}

void getQuestionByTopic(MYSQL *con, char *topic, db_entry qresult)
{
  char query[] = "SELECT * FROM entries where topic='";
  strcat(query, topic);
  strcat(query, "' ORDER BY RAND() LIMIT 1");

  if (mysql_query(con, query))
  {
    finish_with_error(con);
  }

  MYSQL_RES *result = mysql_store_result(con);
  int num_fields = mysql_num_fields(result);
  MYSQL_ROW row;
  while (row = mysql_fetch_row(result))
  {
    // row[0] is id
    strncpy(qresult.qtopic, row[1],1000);
    strncpy(qresult.qtype, row[2],1000);
    strncpy(qresult.qtext, row[3],1000);
    strncpy(qresult.ans, row[4],1000);
    strncpy(qresult.expln, row[5],1000);
  }
}

// MySQL queries end

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

void Give_all_topics_from_databases(char* Output_from_database){
  // provide all topics in string
  getTopics(con,Output_from_database);
  return;
}

void Give_specific_topic_question_from_databases(char* Output_from_database,char* Request){
  // providing a random question from that specific question
  struct db_entry temp;
  getQuestionByTopic(con, Request,temp);
  memset(Output_from_database,0,10000);

  return;
}


char user_name[MAXQUEUE][20];

void Individual_mode(int consockfd,char* name){
  char Request[MAXREQ],Output_from_database[10000],question_from_databases[10000]="default question";
  // topics
  memset(Request,0, MAXREQ);
  n = read(consockfd,Request,MAXREQ-1); /* Recv */
  Give_all_topics_from_databases(Output_from_database);
  if (n <= 0) return;
  n = write(consockfd, Output_from_database, strlen(Output_from_database)); /* echo*/


  // random question from that specific topic
  memset(Request,0, MAXREQ);
  n = read(consockfd,Request,MAXREQ-1); /* Recv */
  Give_specific_topic_question_from_databases(question_from_databases,Request);
  if (n <= 0) return;
  n = write(consockfd, question_from_databases, strlen(question_from_databases)); /* echo*/


  //next step
  memset(Request,0, MAXREQ);
  n = read(consockfd,Request,MAXREQ-1); /* Recv */
  if (n <= 0) return;
  n = write(consockfd, Request, strlen(Request)); /* echo*/
  
  remove_end_character(Request);
  if(strcmp(Request,"n")==0){
    printf("'n' pressed\n");
    Individual_mode(consockfd,name);
  }
  else if(strcmp(Request,"r")==0){
    printf("'r' pressed\n");
    main_menu(consockfd,name);
  }
  return;
}


void main_menu(int consockfd,char* name){
  char option[MAXREQ];
  memset(option,0, MAXREQ);
  n = read(consockfd,option,MAXREQ-1); /* Recv */
  remove_end_character(option);
  printf("user %s Selected %s mode\n", name,option);
  if (n <= 0) return;
  n = write(consockfd, option, strlen(option)); /* echo*/
  if(option[0]=='A'){
    //Admin
  


  }
  else if(option[0]=='I'){
    //Individual
    Individual_mode(consockfd,name);
    
  }
  else{
    //Group
    printf("Active Users are ");
    for(int i=0;i<MAXQUEUE;i++){
      if(user_name[i]){
        printf("%s, ",user_name[i]);
      }
    }
    printf("specify user name with whom you want to collaborate\n");




  }
}

void server(int consockfd,int thread_id) {
  char reqbuf[MAXREQ],name[MAXREQ];

  memset(name,0, MAXREQ);
  n = read(consockfd,name,MAXREQ-1); /* Recv */
  remove_end_character(name);
  printf("user name registored as:%s\n", name);
  if (n <= 0) return;
  n = write(consockfd, name, strlen(name)); /* echo*/
  
  memset(user_name[thread_id],0,20);
  for(int i=0;i<strlen(name);i++){
    user_name[thread_id][i]=name[i];
  }

  main_menu(consockfd,name);

  //crearing user
  memset(user_name[thread_id],0,sizeof(user_name[thread_id]));
  
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

    server(consockfd,thread_id);

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

  con = mysql_init(NULL);
  if (con == NULL)
  {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "root", "", NULL, 0, NULL, 0) == NULL)
  {
    finish_with_error(con);
  }

  select_db(con);



  //************************

  struct db_entry entry[2];

  strncpy(entry[0].ans,"1ans",1000);
  strncpy(entry[0].expln,"1expn",1000);
  strncpy(entry[0].qtext,"1qtext",1000);
  strncpy(entry[0].qtopic,"1qtopic",1000);
  strncpy(entry[0].qtype,"1qtype",1000);
  strncpy(entry[1].ans,"2ans",1000);
  strncpy(entry[1].expln,"2expn",1000);
  strncpy(entry[1].qtext,"2qtext",1000);
  strncpy(entry[1].qtopic,"2qtopic",1000);
  strncpy(entry[1].qtype,"2qtype",1000);
  
  addEntries(con,entry,2);

  //***********************************

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
