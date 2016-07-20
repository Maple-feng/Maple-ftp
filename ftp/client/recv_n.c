#include "fun.h"

int recv_n(int new_fd,char *buf,int len)//循环接受
{
	int ret;
	int total=0;
	while(total<len)
	{
		ret=recv(new_fd,buf+total,len-total,0);
		total=total+ret;
	}
	return 0;
}
