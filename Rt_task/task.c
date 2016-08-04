#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>
#include"quanser.h"

void* task_function(void *arg) 
{
	int indx = (int) arg;
	struct sched_param param;
	pthread_t id = pthread_self();
	param.sched_priority = priority[indx];
    	
    	if (pthread_setschedparam(id, SCHED_FIFO, &param)) 
	{
        	perror("sched_setscheduler failed");
       		exit(-1);
   	}


    	/* Lock memory */

    	if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) 
	{
        	perror("mlockall failed");
        	exit(-2);
    	}



    	/* Pre-fault our stack */

    	stack_prefault();
	
	
	
       printf("Task %d initialization completed \n", indx);
   	
	/*pick a task which would randomly execute side channel*/
	int max=6;
	int min=0;
	int r1 = (rand() % (max + 1 - min)) + min;
	while(r1==0 || r1==6)
	{
		r1 = (rand() % (max + 1 - min)) + min;
	}

	//If it is the controller task 
	if(indx==0)
	{
		controller_task(indx);
	}

	//if it is the Monitoring Unit
	else if(indx==6)
	{
		monitoring_unit(indx);
	}

	//any random task is executing side channel  
	else if(indx==r1)
	{
		side_channel();
	}

	//else call a dummy task
	else
	{
		dummy_task(indx);
	}
	return NULL;
}

