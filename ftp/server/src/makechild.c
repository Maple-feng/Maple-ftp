#include "fun.h"

void makechild(pchild p,int n)
{
	int i;
	int fds[2];
	pid_t pid;
	for(i=0;i<n;i++)
	{
		socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
		pid=fork();
		if(pid==0)
		{
			close(fds[1]);
			signal(SIGPIPE,sig_handle);
			handle(fds[0]);//子进程流程
		}
		close(fds[0]);
		p[i].pid=pid;//获得子进程的pid
		p[i].fdw=fds[1];//保存管道的另一端
		p[i].busy_flg=0;//子进程标示成非忙碌
	}
}
