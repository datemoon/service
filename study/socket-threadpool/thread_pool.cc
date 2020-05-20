

#include "thread_pool.h"
#include <iostream>
#include <deque>
#include <stdlib.h>
#include <pthread.h>

void CTask::SetConnFd(int data)
{
	connfd = data;
}

int CTask::GetConnFd()
{
	return connfd;
}

deque<CTask*> CThreadPool::task_list;         //
bool CThreadPool::shutdown = false;
pthread_mutex_t CThreadPool::pthread_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CThreadPool::pthread_cond = PTHREAD_COND_INITIALIZER;


CThreadPool::CThreadPool(int thread_num)
{
	this->thread_num = thread_num;
	cout << "I will create " << thread_num << " threads." << endl;
	Create();
}



void *CThreadPool::ThreadFunc(void *thread_data)
{
	pthread_t tid = pthread_self();
	while(true)
	{
		pthread_mutex_lock(&pthread_mutex);
		while(task_list.size()==0 && !shutdown)
		{
			pthread_cond_wait(&pthread_cond, &pthread_mutex);
		}

		if(shutdown)
		{
			pthread_mutex_unlock(&pthread_mutex);
			cout << "thread " << tid << " will exit\n" << endl;
			pthread_exit(NULL);
		}
		
		cout << "tid " << tid << " run.\n" << endl;

		CTask *task = task_list.front();
		task_list.pop_front();

		pthread_mutex_unlock(&pthread_mutex);

		task->Run();
	}

	return NULL;
}


int CThreadPool::AddTask(CTask *task)
{
	pthread_mutex_lock( &pthread_mutex);
	this->task_list.push_back(task);
	pthread_mutex_unlock( &pthread_mutex);

	pthread_cond_signal( &pthread_cond);
	return 0;
}


int CThreadPool::Create()  
{
	pthread_id = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
	for(int i =0;i<thread_num; ++i)
	{
		pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);
	}
	return 0;
}


int CThreadPool::StopAll()
{
	if(shutdown)
	{
		return -1;
	}
	cout << "Now I will end all threads!!\n" << endl;

	// wake up all thread distory thread pool
	shutdown = true;
	pthread_cond_broadcast(&pthread_cond);

	for(int i =0 ; i< thread_num ; ++i)
	{
		pthread_join(pthread_id[i], NULL);
	}

	free(pthread_id);
	pthread_id = NULL;

	pthread_mutex_destroy( &pthread_mutex);
	pthread_cond_destroy( &pthread_cond);

	return 0;
}

int CThreadPool::GetTaskSize()
{
	return task_list.size();
}




