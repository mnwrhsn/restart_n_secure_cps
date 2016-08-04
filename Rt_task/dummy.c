#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>
#include<signal.h>
#include"quanser.h"


int interval;

void dummy_task(int indx)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	t.tv_sec++;

	interval= period[indx];

	struct sigaction act;
    	act.sa_handler = sig_handle;
    	sigaction (SIGALRM, & act, 0);

	while(1) {

		ualarm (10000, 0);

            /* wait until next shot */
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
    
            /* do the stuff */
            printf("Task %d \n",indx);
            
            
    
            /* calculate next shot */
            t.tv_nsec += interval;
    
            while (t.tv_nsec >= NSEC_PER_SEC) {
                t.tv_nsec -= NSEC_PER_SEC;
                t.tv_sec++;
            }
            
            printf("Task Sleeping %d \n",indx);
        }

}
