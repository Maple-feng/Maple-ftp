#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/mman.h>

typedef struct{
	pid_t pid;//子进程pid
	int fdw;//相应子进程的写端，socket管道
	short busy_flg;
}child,*pchild;
typedef struct{
	int len;//实际发送数据的长度
	char buf[1100];
}data,*pdata;

void makechild(pchild,int);
int handle(int);
void send_fd(int,int);
void recv_fd(int,int*);
int send_file(int,char*,long);
void sig_handle(int);
int recv_n(int,char*,int);
