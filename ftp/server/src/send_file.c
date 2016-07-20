#include "fun.h"

int send_file(int new_fd,char *filename,long l)
{
	data d;
	bzero(&d,sizeof(d));

//	d.len=strlen(filename);//发送文件名
//	strcpy(d.buf,filename);
//	send(new_fd,&d,4+d.len,0);
	int fd;
	fd=open(filename,O_RDONLY);
	if(-1==fd)
	{
		perror("open");
		return -1;
	}
	lseek(fd,l,SEEK_SET);
	while(bzero(&d,sizeof(d)),(d.len=read(fd,d.buf,sizeof(d.buf)))>0)//发送文件内容
	{
	int	ret=send_n(new_fd,&d,4+d.len);
		if(-1==ret)
		{
			close(new_fd);
			return -1;
		}		
	}
	int flag=0;
	send(new_fd,&flag,4,0);//发送结束标志
//	close(new_fd);
	return 0;
}
