#include <linux/init.h>
#include <linux/module.h> /** needed by all modules **/
#include <linux/kernel.h>  /** This is for KERN_ALERT **/
#include <linux/hrtimer.h>  /** Linux High Resolution Timer **/
#include <linux/ktime.h>

MODULE_LICENSE("GPL");

/** High Resolution timer - for Real time applications**/
static struct hrtimer hrTimer;

/** One should avoid re-starting the timer again in the timer callback **/
/** The design should be state based and should avoid this situation **/
/** On a particular state, it should again restart the timer **/
enum hrtimer_restart hrTimerCallback( struct hrtimer *timer)
{
    

    char * envp[] = { "HOME=/", NULL };
    char * argv[] = { "/sbin/reboot", NULL };



    /** jiffies is a global variable - ticks of the kernel **/
    printk("HR--TIMER..called..%ld\n", jiffies);

    //printk("HR--TIMER..system is rebooting now!..%ld\n", jiffies);

    //call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);


    return HRTIMER_NORESTART;
    //return HRTIMER_RESTART;
}

static int hrTimer_init(void)
{
    ktime_t ktime;
    printk(KERN_ALERT "Hello High Resolution Timer\n");
    /** setup timer **/
    /** 1000 nano seconds **/
    //ktime = ktime_set( 0, 1000 );
    ktime = ktime_set( 10, 0 );
    hrtimer_init( &hrTimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
    hrTimer.function = &hrTimerCallback;
    printk("Starting high resolution timer to start firecallback..\r\n");
    hrtimer_start( &hrTimer, ktime, HRTIMER_MODE_REL );
    return 0;
}

static void hrTimer_exit(void)
{
    int ret = 0;
    printk(KERN_ALERT "Goodbye..HRTimer\n");
    /*** delete timer **/
    ret = hrtimer_cancel( &hrTimer );

    if(ret)
    {
        printk("ERROR in deleting the HR --Timer\r\n");
    }

}

module_init(hrTimer_init);
module_exit(hrTimer_exit);
