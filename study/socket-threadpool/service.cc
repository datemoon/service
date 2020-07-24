#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
 #include <netinet/tcp.h>


#include <unistd.h>
#include <error.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include "thread_pool.h"


class CMyTask:public CTask
{
public:
	CMyTask() = default;
	int Run()
	{
		int connfd = GetConnFd();
		while(true)
		{
			char recvbuf[1024];
			char sendbuf[1024];

			memset(recvbuf,0x00,sizeof(recvbuf));
			memset(sendbuf,0x00, sizeof(sendbuf));

			int len = recv(connfd, recvbuf, sizeof(recvbuf), 0 );

			if(len <= 0 )
			{
				if(errno == EAGAIN || errno == EINPROGRESS)
				{
					printf("EAGAIN=%d EINPROGRESS=%d %d\n",EAGAIN,EINPROGRESS,errno);
					printf("|%d| timeout and continue\n",connfd);
				//	continue;
				}

				printf("on buf.\n");
				break;
			}
			printf("from |%d| receive \"%s\" \n", connfd, recvbuf);
			//printf("Please input: ");
			//fflush(stdout);
			//fgets(sendbuf, 1024,stdin);
			if(strncmp(recvbuf,"end",3) == 0)
			{
				//close(connfd);
				break;
			}
			sprintf(sendbuf, "from |%d| receive \"%s\" \n", connfd, recvbuf);
			send(connfd, sendbuf, sizeof(sendbuf), 0);
			printf("send |%d| : %s ok\n",connfd, sendbuf);
		}
		close(connfd);
		printf("close |%d| ok.\n",connfd);
		return 0;
	}

};

int main(int argc, char *argv[])
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0 );
	assert(sockfd != -1);
	struct sockaddr_in ser, cli;
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = AF_INET;
	inet_aton("127.0.0.1",&ser.sin_addr);
	ser.sin_port = htons(6500);
	int keepalive = 1; // open keepalive
	int keeptime = 5; // 5s no data
	int keepinterval = 1;
	int keepcount = 3;


	setsockopt(sockfd,SOL_SOCKET, SO_KEEPALIVE, (void*)&keepalive,sizeof(keepalive));
	setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keeptime, sizeof(keeptime));
	setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void*)&keepinterval, sizeof(keepinterval));
	setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void*)&keepcount, sizeof(keepcount));
	struct timeval rec_timeout;
	rec_timeout.tv_sec = 1;
	rec_timeout.tv_usec =0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (void*)&rec_timeout, sizeof(rec_timeout));
	int flag = 1;
	int len = sizeof(int);
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1)
	{
		printf("Cannot set socket options!");
		return -1;
	}
	int res=bind(sockfd, (struct sockaddr *) & ser, sizeof(ser));
	assert(res != -1);
	
	int nthread = 2;
	listen(sockfd, nthread);

	CThreadPool pool(nthread);

	while(true)
	{
		socklen_t len = sizeof(cli);
		int connectfd = accept(sockfd, (struct sockaddr *)&cli, &len);
		if(connectfd < 0)
			printf("cli connect failed.\n");
		else
		{
			CTask *ta = new CMyTask;
			ta->SetConnFd(connectfd);
			pool.AddTask(ta);
		}
	}

	close(sockfd);
	return 0;
}

