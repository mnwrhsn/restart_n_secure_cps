#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h>
#include<sys/ioctl.h>
#include"driver_ioctl.h"
 
int main()
{
char buf[100]="hello";
int a=10;
char rbuf[7];
int fd=open("/dev/mydev",O_RDWR);
ioctl(fd,QUERY_SET_VALUE,&a);
printf("=======%d=======",a);
}
