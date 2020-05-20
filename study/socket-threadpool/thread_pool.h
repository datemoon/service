#include <deque>
#include <string>
#include <pthread.h>


using namespace std;

class CTask
{
protected:
	string task_name;
	int connfd;       // connect fd
public:
	CTask() = default;
	CTask(string &task_name): task_name(task_name), connfd(0) { }
	virtual int Run() = 0;
	void SetConnFd(int data); // set socket id
	int GetConnFd();
	virtual ~CTask() { }
};

class CThreadPool
{
private:
	static deque<CTask *> task_list;  // task list
	static bool shutdown;      // thread exit
	int thread_num ;         // start thread number 
	pthread_t *pthread_id;

	static pthread_mutex_t pthread_mutex;
	static pthread_cond_t pthread_cond;

protected:
	static void *ThreadFunc(void *thread_data); // thread callback function
	static int MoveToIdle(pthread_t tid);
	static int MoveToBusy(pthread_t tid);

	int Create();

public:
	CThreadPool(int thread_num=10);
	int AddTask(CTask *task);
	int StopAll();
	int GetTaskSize();

};



