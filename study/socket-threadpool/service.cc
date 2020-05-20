#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>

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
				printf("on buf.\n");
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
		}
		close(connfd);
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

	int res=bind(sockfd, (struct sockaddr *) & ser, sizeof(ser));
	assert(res != -1);

	listen(sockfd, 5);

	CThreadPool pool(5);

	while(true)
	{
		socklen_t len = sizeof(cli);
		int connectfd = accept(sockfd, (struct sockaddr *)&cli, &len);
		if(connectfd < 0)
			printf("cli connect failed.");
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

