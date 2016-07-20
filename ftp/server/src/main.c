#include "fun.h"

int main(int argc,char* argv[])
{
//	if(argc!=4)
//	{
//		printf("error args\n");
//		return -1;
//	}
	int n=5;
	//int n=atoi(argv[3]);//创建子进程个数--传参
	pchild p=(pchild)calloc(n,sizeof(child));
	makechild(p,n);//创建子进程，初始化相应socket管道
	int sfd=socket(AF_INET,SOCK_STREAM,0);//代表的是ipv4网络协议，tcp，传输协议编号，通常为0
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));//结构体清空
	ser.sin_family=AF_INET;//采用ipv4网络协议
	ser.sin_port=htons(atoi("2001"));
	//ser.sin_port=htons(atoi(argv[2]));//端口号传参。需要字符型转成整形
	ser.sin_addr.s_addr=inet_addr("192.168.1.103");
	//ser.sin_addr.s_addr=inet_addr(argv[1]);//将点分十进制转成32位网络字节序
	int ret;
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(ser));//绑定端口号和IP地址
	if(-1==sfd)
	{
		perror("bind");
		return -1;
	}
	listen(sfd,n);//监听n个
	int epfd=epoll_create(1);//得到一个epfd，不使用时要close
	struct epoll_event event;
	struct epoll_event* pevs=(struct epoll_event*)calloc(n+1,sizeof(event));//n个子进程和一个sfd
	event.events=EPOLLIN;//监控描述符是否可读(基本只用可读）
	event.data.fd=sfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);//将sfd添加到监控列表
	int i;
	for(i=0;i<n;i++)//监控管道的fdw端
	{
		event.events=EPOLLIN;
		event.data.fd=p[i].fdw;
		epoll_ctl(epfd,EPOLL_CTL_ADD,p[i].fdw,&event);//将每个写端添加到监控列表
	}
	int new_fd;
	int j;
	int overflg;
	while(1)
	{
		ret=epoll_wait(epfd,pevs,n+1,-1);
		for(i=0;i<ret;i++)
		{
			if(pevs[i].events==EPOLLIN&&sfd==pevs[i].data.fd)
			{
				new_fd=accept(sfd,NULL,NULL);
				if(-1==new_fd)
				{
					perror("accept");
					return -1;
				}
				for(j=0;j<n;j++)//找到空闲的子进程
				{
					if(p[j].busy_flg==0)
						break;
				}
				send_fd(p[j].fdw,new_fd);
				printf("Welcome client\n");
				close(new_fd);
				p[j].busy_flg=1;//子进程变为运行状态
			}
			for(j=0;j<n;j++)
			{
				if(pevs[i].data.fd==p[j].fdw)
				{
					read(p[j].fdw,&overflg,4);//子进程想父进程发的结束，读取出来
					if(overflg==-1)
					{
						printf("client end error\n");
					}
					p[j].busy_flg=0;
				}
			}
		}
	}
	return 0;
}
