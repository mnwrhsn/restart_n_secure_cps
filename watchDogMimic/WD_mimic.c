
#include <linux/init.h>
#include <linux/module.h> /** needed by all modules **/
#include <linux/kernel.h>  /** This is for KERN_ALERT **/
#include <linux/hrtimer.h>  /** Linux High Resolution Timer **/
#include <linux/ktime.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include<linux/fs.h>
#include<linux/string.h>
#include<asm/uaccess.h>
#include<linux/ioctl.h>
#include"driver_ioctl.h"



MODULE_LICENSE("GPL");

static char ker_buf[100]; //driver local buffer

int a;
static ktime_t ktime;
static int dev_open(struct inode *inod, struct file *fil);
static ssize_t dev_read(struct file *filep,char *buf,size_t len,loff_t *off);
static ssize_t dev_write(struct file *flip,const char *buff,size_t len,loff_t *off);
static int dev_release(struct inode *inod,struct file *fil);
//structure containing device operation
 
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{

switch(cmd)
{
case QUERY_GET_VALUE:
copy_to_user((int*)arg,&a,sizeof(int));
break;
 
case QUERY_SET_VALUE:
copy_from_user(&a,(int *)arg,sizeof(int));
break;
 
case QUERY_CLEAR_VALUE:
break;
}
return 0;
}
static struct file_operations fops=
{
.read=dev_read, //pointer to device read function
.write=dev_write, //pointer to device write function
.open=dev_open, //pointer to device open function
.release=dev_release, //pointer to device release function
.unlocked_ioctl=my_ioctl, //ioctl added by me
};

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

    if(a==10)
	{
		hrtimer_forward_now(timer, ktime);
		return HRTIMER_RESTART;
	}
    
    else 
	{
		
		return HRTIMER_NORESTART;
    		//return HRTIMER_RESTART;
	}
		
}

static int hrTimer_init(void)
{
   int t=register_chrdev(90,"mydev",&fops);
	if(t<0)
	printk(KERN_ALERT "device registration failed.");
	else
	printk(KERN_ALERT "device registred\n");
	return 0;
    
    printk(KERN_ALERT "Hello High Resolution Timer\n");
    /** setup timer **/
    /** 1000 nano seconds **/
    //ktime = ktime_set( 0, 1000000 );
    ktime = ktime_set( 10, 0 );
    hrtimer_init( &hrTimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
    hrTimer.function = &hrTimerCallback;
    printk("Starting high resolution timer to start firecallback..\r\n");
    hrtimer_start( &hrTimer, ktime, HRTIMER_MODE_REL );
    return 0;
}

static void hrTimer_exit(void)
{
	unregister_chrdev(90,"mydev");
	printk(KERN_ALERT "exit");
    int ret = 0;
    printk(KERN_ALERT "Goodbye..HRTimer\n");
    /*** delete timer **/
    ret = hrtimer_cancel( &hrTimer );

    if(ret)
    {
        printk("ERROR in deleting the HR --Timer\r\n");
    }

}

static int dev_open(struct inode *inod, struct file *fil)
{
printk("KERN_ALERT device opened");
return 0;
}
 
static ssize_t dev_read(struct file *filep,char *buf,size_t len,loff_t *off)
{
copy_to_user(buf,ker_buf,len);
return len;
}
 
static ssize_t dev_write(struct file *flip,const char *buf,size_t len,loff_t *off)
{
copy_from_user(ker_buf,buf,len);
ker_buf[len]=0;
return len;
}
 
static int dev_release(struct inode *inod,struct file *fil)
{
printk("KERN_ALERT device closed\n");
return 0;
}

module_init(hrTimer_init);
module_exit(hrTimer_exit);
