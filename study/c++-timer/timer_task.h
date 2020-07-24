#ifndef __TIMER_TASK_H__
#define __TIMER_TASK_H__

#include <signal.h>
#include <sys/time.h>
#include <stdio.h>

void SighandlerT(int m )
{
	m++;
	printf("%d\n",m);
	if(m> 3)
	{
		printf("time out.\n");
	}
}

void SighandlerT1(const int signal, void *ptr );

class TimerInterval
{
public:
	TimerInterval()
	{
		count_ = 0;
	}

	void SetTimer()
	{
		struct itimerval itv;
		itv.it_interval.tv_sec = 1;
		itv.it_interval.tv_usec = 0;
		itv.it_value.tv_sec = 1;
		itv.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &itv, &itv_);
	}


	void Init()
	{
		SetTimer();
		signal(SIGALRM, (void (*)(int))SighandlerT1);
		SighandlerT1(SIGALRM,(void*)this);
	}

	~TimerInterval(){;}
	int count_;
private:
	struct itimerval itv_;
};

void SighandlerT1(const int signal, void *ptr )
{
	TimerInterval *tmp = static_cast<TimerInterval*>(ptr);
	tmp->count_++;
	printf("%d\n",tmp->count_);
	if(tmp->count_ > 3)
	{
		printf("time out.\n");
	}
}
#endif
