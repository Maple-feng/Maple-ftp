#include "fun.h"

int send_n(int new_fd,char *buf,int len)//循环发送
{
	int ret;
	int total=0;
	while(total<len)
	{
		ret=send(new_fd,buf+total,len-total,0);
		if(ret<0)
		{
			return -1;
		}
		total=total+ret;
	}
	return 0;
}
