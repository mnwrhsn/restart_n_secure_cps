#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>
#include"quanser.h"

pthread_t tid[NTASK]; //multithreading for various real time task.
long long int period[NTASK] = {10000000, 15000000, 20000000, 50000000, 70000000, 100000000, 2000000000}; //period of each task
int priority[NTASK] = {98, 97, 96, 95, 94, 93, 92}; //priority of each task.


void stack_prefault(void) {

        unsigned char dummy[MAX_SAFE_STACK];

        memset(dummy, 0, MAX_SAFE_STACK);
        return;
}

static inline void tsnorm(struct timespec *ts) {
    while (ts->tv_nsec >= NSEC_PER_SEC) {
        ts->tv_nsec -= NSEC_PER_SEC;
        ts->tv_sec++;
    }
}

int main(int argc, char* argv[])
{
	int i,err;
        struct sched_param param;

        /* Declare ourself as a real time task */

        param.sched_priority = MY_PRIORITY;
        if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
                perror("sched_setscheduler failed");
                exit(-1);
        }

        /* Lock memory */

        if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
                perror("mlockall failed");
                exit(-2);
        }

        /* Pre-fault our stack */

        stack_prefault();
	
	for (i = 0; i < NTASK; i++) 
	{
        	err = pthread_create(&(tid[i]), NULL, task_function, (void*) i);

        	/* Some error checking */

       		 if (err != 0)
            		printf("Can't create thread :[%s]", strerror(err));
       		 else
           		printf("Thread created successfully\n");
    	}
	for (i = 0; i < NTASK; i++)
	{
        	pthread_join(tid[i], NULL);
    	}

}
