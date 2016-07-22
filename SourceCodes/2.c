#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include<signal.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/stat.h>
#include"quanser.h"
#include <linux/watchdog.h>
 

int sockfd = 0;

void stack_prefault(void) {

    unsigned char dummy[MAX_SAFE_STACK];

    memset(dummy, 0, MAX_SAFE_STACK);
    return;
}

void Sa_handler(int signo)
{
    printf("signal caught");
    close(sockfd);
    signal(SIGINT,Sa_handler);
}

int main()
{
	struct sched_param param;
	param.sched_priority = 97;
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    //if (pthread_setschedparam(0, SCHED_FIFO, &param)) {
        perror("sched_setscheduler failed");
        exit(-1);
    }
	/* Lock memory */

    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed");
        exit(-2);
    }
	/* Pre-fault our stack */

    stack_prefault();

	char *dev;
	dev = WATCHDOGDEV;
	int fd = open(dev, O_RDWR);
	int interval=1;

  int n = 0;

  struct sockaddr_in serv_addr;
  char *message ;
  int num;

	signal(SIGPIPE, SIG_IGN);
  signal(SIGINT,Sa_handler);
  char recvBuff[68];
  char sendBuff[68];
  memset(recvBuff, '0',sizeof(recvBuff));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Error : Could not create socket \n");
    return -1 ;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5000);

  if(inet_pton(AF_INET,"192.168.7.1", &serv_addr.sin_addr)<=0)
  {
    printf("\n inet_pton error occured\n");
    return -1;
  }
  if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\n Error : Connect Failed \n");
    return -1;
  }
  if (ioctl(fd, WDIOC_SETTIMEOUT, &interval) != 0) {
         fprintf(stderr,
            "Error: Set watchdog interval failed\n");
         exit(EXIT_FAILURE);
	}
  while(1)
  {
	ioctl(fd, WDIOC_KEEPALIVE, NULL);
         fprintf(stdout, "Kick watchdog through IOCTL\n");
    struct state st;
    signal(SIGINT,Sa_handler);
    int k,g;
    
    
	memset(recvBuff, '0', sizeof(recvBuff));
	if((num = recv(sockfd, &k, sizeof(k),0))<=0)
        {
            printf("Either Connection Closed or Error\n");
            close(sockfd);
            exit(1);
        }
	printf("k=%d \n",k);
        if((num = recv(sockfd, recvBuff, k,0))<=0)
        {
            printf("Either Connection Closed or Error\n");
            close(sockfd);
            exit(1);
        }

        recvBuff[num] = '\0';
        printf("Client:Message Received From Server -  %s\n",recvBuff);
        sscanf(strtok(recvBuff , ", "), "%lf", &st.elevation);
        sscanf(strtok(NULL, ", "), "%lf",&st.pitch);
       	sscanf(strtok(NULL, ", "), "%lf",&st.travel);
	sscanf(strtok(NULL, ", "), "%lf",&st.d_elevation);
        sscanf(strtok(NULL, ", "), "%lf",&st.d_pitch);
    	sscanf(strtok(NULL, ", "), "%lf",&st.d_travel);
	printf("client:state received \n");
	printf("elevation %lf\n",st.elevation);
	printf("pitch %lf\n",st.pitch);
	printf("travel %lf\n",st.travel);
	printf("d_elevation %lf\n",st.d_elevation);
	printf("d_pitch %lf\n",st.d_pitch);
	printf("d_travel %lf\n",st.d_travel);
	struct command comm= controller_safety(st);
	printf(" u1= %d  u2= %d  \n",comm.u1,comm.u2); 
	sprintf(sendBuff,"complex, %f, %f, ",comm.u1,comm.u2); 
	g=strlen(sendBuff);
	if ((send(sockfd,&g,sizeof(int),0))== -1)
        {
                 fprintf(stderr, "Failure Sending Message\n");
                 close(sockfd);
                 break;
        }
	if((send(sockfd,sendBuff,g,0))==-1)
      	{
        	printf("Failure in sending Message\n");
        	close(sockfd);
        	exit(1);
        }
      
        
    }
    close(sockfd);
    return -1;
}
