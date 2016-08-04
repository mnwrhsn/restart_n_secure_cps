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

pthread_t thread_id; //Multithreading
time_t now;
int milliseconds=10; //time after which each process is invoked
char sendBuff[1025]; // the state of the device sent to both the controllers
char recvBuff[68]; //commands from both the controllers
char type[68]; // command from complex unit or safety unit
struct state st; //current state of the helicopter
struct command safety; //last safety controller command
struct command complex; //last complex cotroller commands
int num;
int count=0;
int num_t=0;
int flag=0;
pthread_cond_t      condA  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;

struct arg_struct 
{
    int arg1;
    int arg2;
};

void *connection_handler(void *);
void *decision_module(void *);


int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0, c;
    struct sockaddr_in serv_addr, client;
    
    /*initial state of the helicopter*/
    st.elevation=0;
    st.pitch=0;
    st.travel=0;
    st.d_elevation=0;
    st.d_pitch=0;
    st.d_travel=0;


    
    time_t ticks;


    //int i;


    //Create socket with a TCP connection
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
   
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));
  
    //Prepare the sockaddr_in structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);
	
   /*bind*/
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

   //listen for a connection request
    listen(listenfd, 10);
    

    while(1)
    {
	/*ignore the sigPIPE signal if raised*/
	signal(SIGPIPE, SIG_IGN);

	//accept incoming connection
        connfd = accept(listenfd, (struct sockaddr*)&client, (socklen_t*)&c);

	/*structure passed to the target function of each thread
	structure contains socket descriptor and count of the number of threads created*/

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

//targeted thread program for every controller which the server executes
void *connection_handler(void *socket_desc)
{
	
	num_t =num_t +1;//thread count

	/*ignore all signals*/
	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask, NULL);


	struct arg_struct *args = socket_desc;
  	int connfd = args -> arg1;//get the socket descriptor
	int count = args -> arg2; //thread number

	/*convert the state of the device into one string(recvBuff) to be sent through a socket*/

  	sprintf(recvBuff,"%f, %f, %f, %f, %f, %f, ",st.elevation, st.pitch,st.travel,st.d_elevation,st.d_pitch,st.d_travel);

	/*if atleast one connection is established invoke the decision handler*/
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

		/* k = length of the string containing of state of the system
		g = length of the string containing of command received */
	  	int k,g;
	 	printf("string length = %d\n ",strlen(recvBuff));
	  	k=strlen(recvBuff);//string length of the state of the system

	  	if ((send(connfd,&k,sizeof(int),0))== -1) //sending string length of state
            	{
                 	fprintf(stderr, "Failure Sending Message\n");
                 	close(connfd); //closing the connection
			num_t --; //decreasing thread count
			printf("num_t %d= \n",num_t); 
			pthread_cond_signal(&condA);
                 	pthread_exit(NULL); //thread exit
		
            	}
      	  	if ((send(connfd,recvBuff, k,0))== -1) //sending current state of the system
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
	    	if((num = recv(connfd, &g, sizeof(g),0))<=0) //receiving the length of the controller command
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
		/*if command is received from the safety controller*/
		if(strcmp("safety",type)==0)
		{
			/*update the last received safety controller commands*/
			sscanf(strtok(NULL, ", "), "%lf",&safety.u1);
        		sscanf(strtok(NULL, ", "), "%lf",&safety.u2);
		}
		/*if command is received from the complex controller*/
		else if(strcmp("complex",type)==0)
		{
			

			now = time(0);	//update the time stamp when the last controller command was received	
			/*update the last received complex controller commands*/	
			sscanf(strtok(NULL, ", "), "%lf",&complex.u1);
        		sscanf(strtok(NULL, ", "), "%lf",&complex.u2);
		}
		
		printf("type = %s",type);
		//the server code executes after every 10 milliseconds
		usleep(milliseconds * 1000);

	}
          close(connfd); //close connection
          sleep(1);
}


/* decision module */
void *decision_module(void *arg)
{
	struct command com;
	while(1){
	/*executes every 10 milliseconds*/
	usleep(milliseconds * 1000);

	//print the last safety and complex controller commands received
	printf(" safety u1= %f  u2= %f  \n",safety.u1,safety.u2);
	printf(" complex u1= %f  u2= %f  \n",complex.u1,complex.u2);
	printf("desision module\n");

	/*current time stamp*/
	time_t base = time (0);

	/*if the complex controller command last received was not within the 10 millisecond deadline,
	then safety controller command is executed*/
	if(base-now >milliseconds/1000)
	{
		printf("pick safety\n");
		printf("deadline missed\n");
		//simluating the next state of the machine after safety controller commands are executed
		st= simulate_fixed_control(st,safety,0.001);
	}
	
	/*else the decision module function decides whether the safety controller commands should execute or the complex controller*/
	else
	{
		printf("decide\n");
		//com is the command picked by the decision module
		com = decide(safety,complex,st);
		//simluating the next state of the machine after controller commands are executed
		st= simulate_fixed_control(st,com,0.001);
	}

	/*printing the new state of the machine*/
	printf("elevation %lf\n",st.elevation);
	printf("pitch %lf\n",st.pitch);
	printf("travel %lf\n",st.travel);
	printf("d_elevation %lf\n",st.d_elevation);
	printf("d_pitch %lf\n",st.d_pitch);
	printf("d_travel %lf\n",st.d_travel);
	
	/*converting the state into string to send over the socket*/
	sprintf(recvBuff,"%f, %f, %f, %f, %f, %f, ",st.elevation, st.pitch,st.travel,st.d_elevation,st.d_pitch,st.d_travel);
	
	}
	return NULL;
	
}

