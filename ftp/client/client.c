#include "fun.h"

int main(int argc,char* argv[])
{
//	if(argc!=3)
//	{
//		printf("error args\n");
//		return -1;
//	}
	struct stat s;
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi("2001"));
//	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr("192.168.1.103");
//	ser.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=connect(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("connect");
		return -1;
	}else{
		system("reset");
		printf("Welcome\n");
	}
	int len;
	int len_buf;
	int len_ord;
	int len_file;
	char buf[2000];
	char ord[20]={0};
	char filename[300]={0};
	while(1)
	{
		memset(buf,0,sizeof(buf));
		printf("账号:\n");
		fgets(buf,sizeof(buf),stdin);
		len_buf=strlen(buf)-1;
		send(sfd,&len_buf,4,0);
		send(sfd,buf,len_buf,0);
		memset(buf,0,sizeof(buf));
		printf("请输入密码:\n");
		fgets(buf,sizeof(buf),stdin);
		len_buf=strlen(buf)-1;
		send(sfd,&len_buf,4,0);
		send(sfd,buf,len_buf,0);
		memset(buf,0,sizeof(buf));
		recv(sfd,&len_buf,4,0);
 		recv(sfd,buf,len_buf,0);
		printf("%s\n",buf);
		if(strcmp("验证成功!",buf)==0)
		{
			while(1)
			{	
				memset(ord,0,sizeof(ord));
				memset(buf,0,sizeof(buf));
				memset(filename,0,sizeof(filename));
				fgets(buf,sizeof(buf),stdin);//从键盘读入字符串到buf
				int n,m;
				for(m=0,n=0;buf[m]!='\0';m++)
				{
					while(buf[m]!=' '&&buf[m]!='\0'&&buf[m]!='\n')
					{
						ord[m]=buf[m];
						m++;
					}
					m++;
					while(buf[m]!=' '&&buf[m]!='\0'&&buf[m]!='\n')
					{
						filename[n]=buf[m];
						n++;
						m++;
					}
				}
				len_ord=strlen(ord);
				int ret2=send(sfd,&len_ord,4,0);
				if(-1==ret2)
				{
					perror("send");
					return -1;
				}
				int ret1=0;
				if(len_ord>0)
				{
					ret1=send(sfd,ord,len_ord,0);//发送指令到服务端
					if(ret1==-1)
					{
						perror("send");
						return -1;
					}
				}	
				len_file=strlen(filename);
				ret=send(sfd,&len_file,4,0);
				if(ret==-1)
				{
					perror("send");
					return -1;
				}
				if(len_file>0)
				{
					ret=send(sfd,filename,len_file,0);//发送文件名到服务端
					if(ret==-1)
					{
						perror("send");
						return -1;
					}
				}
				if(ret1>0)
				{	
					if(strcmp("cd",ord)==0)//进入对应目录
					{
						recv(sfd,&len_buf,4,0);
						if(len_buf>0)
						{
							recv(sfd,&buf,len_buf,0);
						}
					}
					else if(strcmp("ls",ord)==0)//列出相应目录文件
					{
						recv(sfd,&len_buf,4,0);
						recv(sfd,&buf,len_buf,0);
						printf("%s",buf);
					}
					else if(strcmp("puts",ord)==0)//将本地文件上传至服务器
					{
						printf("文件上传中，请等待，请勿断开连接\n");
						send_file(sfd,filename);//向服务器上传文件
						printf("文件成功上传至服务器\n");
					}
					else if(strcmp("remove",ord)==0)//删除服务器上的文件
					{
						recv(sfd,&len_buf,4,0);
						recv(sfd,&buf,len_buf,0);
					}
					else if(strcmp("pwd",ord)==0)//显示目前所在路径
					{
						recv(sfd,&len_buf,4,0);
						recv(sfd,&buf,len_buf,0);
						printf("%s\n",buf);
					}
					else if(strcmp("gets",ord)==0)//下载文件到本地
					{
						long server_file;
						recv(sfd,&server_file,sizeof(server_file),0);
						if(server_file>=104857600)//映射传递下载文件
						{	
							printf("使用映射高速下载，请等待，请勿断开连接\n");
							int fd;
							fd=open(filename,O_RDWR|O_CREAT,0666);
							if(-1==fd)
							{
								perror("open");
								return -1;
							}
							
							while(1)
							{
								recv(sfd,&len,sizeof(len),0);
								memset(buf,0,sizeof(buf));
								if(len>0)
								{
									recv_n(sfd,buf,len);
									write(fd,buf,len);
								}else{
									break;
								}
							}
							close(fd);
							printf("高速下载成功\n");
						}else{//普通下载服务器文件至本地
							printf("正在下载，请等待，请勿断开连接\n");
						//	ret=recv(sfd,&len,sizeof(len),0);//接收文件名
						//	if(ret<0)
						//	{
						//		perror("recv");
						//		return -1;
						//	}
							memset(buf,0,sizeof(buf));
						//	recv(sfd,buf,len,0);
						//	printf("%d,%s\n",len,buf);
							int fd;
							fd=open(filename,O_RDWR|O_CREAT,0666);
							if(-1==fd)
							{
								perror("open");
								return -1;
							}		
			
							int ret=stat(filename,&s);
							if(ret==-1)
							{
								perror("stat");
								return -1;
							}
							long sizefile=s.st_size;
							send(sfd,&sizefile,sizeof(sizefile),0);	
                         	lseek(fd,sizefile,SEEK_SET);
							while(1)//接受文件内容
							{
								recv(sfd,&len,sizeof(len),0);//先接收火车头，长度
								memset(buf,0,sizeof(buf));
								if(len >0)//send结束表示发送的对应len是0
								{
									recv_n(sfd,buf,len);//再接内容
									write(fd,buf,len);
								}else{
									break;
								}
							}
							close(fd);
							printf("下载成功\n");
						}
					}
					else if(strcmp("exit",ord)==0)//退出
					{
						printf("欢迎再次使用\n");
						close(sfd);
						return 0;
					}
					else{
						printf("输入有误，请核对后输入\n");
					}
				}
			}
		}else{
			printf("验证失败!\n");
			break;
		}
	}
	close(sfd);
	return 0;
}
