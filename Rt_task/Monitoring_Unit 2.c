#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>

#define MY_PRIORITY (49) /* we use 49 as the PRREMPT_RT use 50
                            as the priority of kernel tasklets
                            and interrupt handler by default */

#define MAX_SAFE_STACK (8*1024) /* The maximum stack size which is
                                   guaranteed safe to access without
                                   faulting */

#define NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */

void stack_prefault(void) {

        unsigned char dummy[MAX_SAFE_STACK];

        memset(dummy, 0, MAX_SAFE_STACK);
        return;
}

int main(int argc, char* argv[])
{
	
        struct timespec t;
        struct sched_param param;
        int interval = 2000000000; /* 2s*/

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

	FILE *fp1 = fopen("/proc/modules", "r");
        if (!fp1) {
                perror("Opening original module");
                exit(1);
        }
	char * line = NULL;
	char a[100][60];
	int i=0;
	ssize_t read;
	size_t len = 0;
	 while ((read = getline(&line, &len, fp1)) != -1)
	{
        	printf("Retrieved line of length %zu :\n", read);
        	printf("%s", line);
		strcpy(a[i], strtok (line," "));
		printf("%s\n",a[i]);
		printf("i=%d",i);
		i++;
    	}
	printf("i=%d",i);
    	fclose(fp1);    
	
        clock_gettime(CLOCK_MONOTONIC ,&t);
        /* start after one second */
        t.tv_sec++;

        while(1) {
                /* wait until next shot */
                clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		printf("loop starts");

                /* do the stuff */
                FILE *current_module = fopen("/proc/modules", "r");
                if (!current_module) 
                {
                        perror("Opening current");
                        exit(1);
                }
		int j=0;
		char * line2 = NULL;
		size_t len2 = 0;
                while(j<i)
		{       
			printf("j=%d \n",j);
                        printf("original %s\n",a[j]);
			if((read = getline(&line2, &len2, current_module)) != -1)
			{
                        	printf("current %s\n",strtok (line2," "));
                        	if(strcmp(a[j], strtok (line2," ")) != 0)
                        	{
                                	system("reboot");
                        	}
			}
			j++;
                }
		
		if (line2)
        		free(line2);
		fclose(current_module);

                /* calculate next shot */
                t.tv_nsec += interval;

                while (t.tv_nsec >= NSEC_PER_SEC) {
                       t.tv_nsec -= NSEC_PER_SEC;
                        t.tv_sec++;
                }
   }
}

