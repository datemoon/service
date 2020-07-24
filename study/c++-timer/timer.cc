#include <stdio.h>
#include "timer_task.h"
#include <unistd.h>
int main(int argc,char *argv[])
{
	TimerInterval timerinv;
	timerinv.Init();
	while(1)
		sleep(1);
	return 0;
}
