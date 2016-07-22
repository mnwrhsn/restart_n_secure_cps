#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include<pthread.h>
#include<signal.h>
#include"quanser.h"

pthread_t thread_id;
time_t now;
int milliseconds=10000;
char sendBuff[1025];
char recvBuff[68];
char type[68];
struct state st;
struct command safety;
struct command complex;
int num;
int count=0;
int num_t=0;
int flag=0;
pthread_cond_t      condA  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;

struct arg_struct {
    int arg1;
    int arg2;
};

void *connection_handler(void *);
void *decision_module(void *);


int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0, c;
    struct sockaddr_in serv_addr, client;
    
    
    st.elevation=0;
    st.pitch=0;
    st.travel=0;
    st.d_elevation=0;
    st.d_pitch=0;
    st.d_travel=0;


    
    time_t ticks;


    //int i;



    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);
    

    while(1)
    {
	signal(SIGPIPE, SIG_IGN);
        connfd = accept(listenfd, (struct sockaddr*)&client, (socklen_t*)&c);
	struct arg_struct a;
	a.arg1=connfd;
	a.arg2=count;
	count++;
	if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &a) < 0)
        {
            perror("could not create thread");
            return 1;
        }
	
     }
      /*close(listenfd);
      return 0;*/
        
   }


void *connection_handler(void *socket_desc)
{
	
	num_t =num_t +1;
	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask, NULL);
	struct arg_struct *args = socket_desc;
  	int connfd = args -> arg1;
	int count = args -> arg2;
  	sprintf(recvBuff,"%f, %f, %f, %f, %f, %f, ",st.elevation, st.pitch,st.travel,st.d_elevation,st.d_pitch,st.d_travel);
	if(count==0)
			if( pthread_create( &thread_id , NULL ,  decision_module , NULL) < 0)
       			 {
            			perror("could not create decision module");
            			pthread_exit(NULL);
         		 }
        while(1)
       //for(i=0;i<2;i++)
        {
	signal(SIGPIPE, SIG_IGN);		
	printf("connfd= %d\n ",connfd);
	printf("count= %d \n",count);
	printf("num_t %d= \n",num_t);
	  int k,g;
	  printf("string length = %d\n ",strlen(recvBuff));
	  k=strlen(recvBuff);
	  if ((send(connfd,&k,sizeof(int),0))== -1)
            {
                 fprintf(stderr, "Failure Sending Message\n");
                 close(connfd);
		num_t --;
		printf("num_t %d= \n",num_t);
		pthread_cond_signal(&condA);
                 pthread_exit(NULL);
		
            }
      	  if ((send(connfd,recvBuff, k,0))== -1)
            {
                 fprintf(stderr, "Failure Sending Message\n");
                 close(connfd);
		count --;
		printf("num_t %d= \n",num_t);
		pthread_cond_signal(&condA);
                 pthread_exit(NULL);
		 
            }
            else
            {
                printf("Server:Msg being sent:\n");
                printf("%s\n",recvBuff);
            }
	    if((num = recv(connfd, &g, sizeof(g),0))<=0)
       	 {
            printf("Either Connection Closed or Error\n");
            close(connfd);
	   num_t --;
	printf("num_t %d= \n",num_t);
	pthread_cond_signal(&condA);
            pthread_exit(NULL);
        }
	printf("g=%d \n",g);
          if ((num = recv(connfd, sendBuff,g,0))== -1)
           {
             perror("recv");
	     num_t --;
	     printf("num_t %d= \n",num_t);
		pthread_cond_signal(&condA);
             pthread_exit(NULL);
           }
          
            sendBuff[num] = '\0';
            printf("Server:Msg Received %s\n", sendBuff);
		strcpy(type,strtok(sendBuff , ", "));
		if(strcmp("safety",type)==0)
		{
			sscanf(strtok(NULL, ", "), "%lf",&safety.u1);
        		sscanf(strtok(NULL, ", "), "%lf",&safety.u2);
		}
		else
		{
			

			now = time(0);			
			sscanf(strtok(NULL, ", "), "%lf",&complex.u1);
        		sscanf(strtok(NULL, ", "), "%lf",&complex.u2);
		}
		printf("type = %s",type);
		usleep(milliseconds * 1000);

	}
          close(connfd);
          sleep(1);
}



void *decision_module(void *arg)
{
	struct command com;
	while(1){
	usleep(milliseconds * 1000);
	printf(" safety u1= %f  u2= %f  \n",safety.u1,safety.u2);
	printf(" complex u1= %f  u2= %f  \n",complex.u1,complex.u2);
	printf("desision module\n");
	time_t base = time (0);
	if(base-now >milliseconds/1000)
	{
		printf("pick safety\n");
		st= simulate_fixed_control(st,safety,0.001);
	}
	else
	{
		printf("decide\n");
		com = decide(safety,complex,st);
		st= simulate_fixed_control(st,com,0.001);
	}
	printf("elevation %lf\n",st.elevation);
	printf("pitch %lf\n",st.pitch);
	printf("travel %lf\n",st.travel);
	printf("d_elevation %lf\n",st.d_elevation);
	printf("d_pitch %lf\n",st.d_pitch);
	printf("d_travel %lf\n",st.d_travel);
	sprintf(recvBuff,"%f, %f, %f, %f, %f, %f, ",st.elevation, st.pitch,st.travel,st.d_elevation,st.d_pitch,st.d_travel);
	}
	return NULL;
	
}

