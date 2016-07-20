#include "fun.h"

int handle(int fdr)//子进程流程
{	
	int fd_log;	
	int len;
	int len_buf;
	int len_ord;
	int len_file;
	char buf_log[100]={0};
	char buf[2000]={0};
	char ord[20]={0};
	char filename[300]={0};
	int new_fd;
	int overflg=0;
	time_t t;
	struct stat s;

	while(1)
	{
		recv_fd(fdr,&new_fd);//接收描述符控制信息
		memset(buf,0,sizeof(buf));
		memset(ord,0,sizeof(buf));
		recv(new_fd,&len_ord,4,0);
		recv(new_fd,ord,len_ord,0);
		recv(new_fd,&len_buf,4,0);
		recv(new_fd,buf,len_buf,0);
		int ret_pass=passwd(buf,ord);
		len_buf=strlen(buf);
		send(new_fd,&len_buf,4,0);
		send(new_fd,buf,strlen(buf),0);
		if(ret_pass==1)
		{
			fd_log=open("log",O_RDWR|O_CREAT|O_APPEND,0600);
			time(&t);
			sprintf(buf_log,"%-10d成功连接       %-20s",new_fd,ctime(&t));
			write(fd_log,buf_log,strlen(buf_log));
			close(fd_log);
			while(1)
			{	
				len_ord=0;
				len_file=0;	
				memset(buf_log,0,sizeof(buf_log));
				memset(buf,0,sizeof(buf));
				memset(ord,0,sizeof(ord));
				memset(filename,0,sizeof(filename));	
				int ret1=0;
				recv(new_fd,&len_ord,4,0);
				if(len_ord>0)
				{
					ret1=recv(new_fd,ord,len_ord,0);//接收指令
					if(ret1==-1)
					{
						perror("recv1");
						return -1;
					}
				}	
				recv(new_fd,&len_file,4,0);
				if(len_file>0)
				{
					recv(new_fd,filename,len_file,0);//接收文件名
				}
				if(ret1>0)
				{	
					fd_log=open("log",O_RDWR|O_CREAT|O_APPEND,0600);
					time(&t);
					sprintf(buf_log,"%-10d%-5s%-10s%-20s",new_fd,ord,filename,ctime(&t));
					write(fd_log,buf_log,strlen(buf_log));
					close(fd_log);
	
					if(strcmp("cd",ord)==0)//进入对应目录
					{
						 cd_handle(filename,buf); 
						 len_buf=strlen(buf);
						 send(new_fd,&len_buf,4,0);
						 if(len_buf>0)
						 {
					     	send(new_fd,buf,len_buf,0);
					     }	
					}
					else if(strcmp("ls",ord)==0)//列出相应目录文件
					{
						 if(strcmp("-l",filename)==0)//ls -l功能
						 {
							lsl_handle(buf);
							len_buf=strlen(buf);
							send(new_fd,&len_buf,4,0);
							send(new_fd,buf,len_buf,0);
						 }
						 else//ls 功能
						 {
							ls_handle(buf);
							len_buf=strlen(buf);
							send(new_fd,&len_buf,4,0);
							send(new_fd,buf,len_buf,0);
						 }
					}
					else if(strcmp("gets",ord)==0)//映射传递文件
					{	
						int ret=stat(filename,&s);
						if(ret==-1)
						{
							perror("stat");
							return -1;
						}
						long sizefile=s.st_size;
						send(new_fd,&sizefile,sizeof(sizefile),0);
                        
						if(sizefile>=104857600)
						{
							int mmap_fd;
							mmap_fd=open(filename,O_RDWR|O_CREAT,0755);
							if(-1==mmap_fd)
							{
								perror("open mmap");
								return -1;
							}
							char *p;
							p=(char*)mmap(NULL,sizefile,PROT_READ|PROT_WRITE,MAP_SHARED,mmap_fd,0);
							if((char*)-1==p)
							{
								perror("mmap");
								return -1;
							}
							data d;
							long long total=0;
							while(total<sizefile)//发送文件内容
							{
								bzero(&d,sizeof(d));
								if(total+1000<=sizefile)
								{
									memcpy(d.buf,p+total,1000);
									d.len=1000;
									int ret=send(new_fd,&d,4+1000,0);
									if(-1==ret)
									{
										close(new_fd);
										return -1;
									}
								}else{
									memcpy(d.buf,p+total,sizefile-total);
									d.len=sizefile-total;
									int ret=send(new_fd,&d,4+sizefile-total,0);
									if(-1==ret)
									{
										close(new_fd);
										return -1;	
									}
								}
								total+=1000;
							}
							int flag=0;
							send(new_fd,&flag,4,0);//发送结束标志
							close(mmap_fd);
						}else{
							long client_len;
							recv(new_fd,&client_len,sizeof(client_len),0);
							printf("%ld",client_len);
							send_file(new_fd,filename,client_len);//向客户端发送文件
						}
					}
					else if(strcmp("puts",ord)==0)//将本地文件上传至服务器
					{
						 
						 printf("有文件在上传至服务器\n");
					     int ret=recv(new_fd,&len,sizeof(len),0);//接收文件名
						 if(ret<0)
						 {
							perror("recv2");
					 		return -1;
						 }
						 memset(buf,0,sizeof(buf));
						 recv(new_fd,buf,len,0);
						 int fd;
						 fd=open(buf,O_RDWR|O_CREAT,0666);
						 if(-1==fd)
					     {
							perror("open");
					 		return -1;
						 }		
	
					 	 while(1)//接受文件内容
						 {
							recv(new_fd,&len,sizeof(len),0);//先接收火车头，长度
							memset(buf,0,sizeof(buf));
							if(len >0)//send结束表示发送的对应len是0
							{
								recv_n(new_fd,buf,len);//再接内容
								write(fd,buf,len);
						 	}else{
								break;
							}
						 }
						 close(fd);
					 }
					else if(strcmp("remove",ord)==0)//删除服务器上的文件
					{
						 remove_handle(buf,filename);
						 ls_handle(buf);
						 len_buf=strlen(buf);
						 send(new_fd,&len_buf,4,0);
						 send(new_fd,buf,len_buf,0);
					}
					else if(strcmp("pwd",ord)==0)//显示目前所在路径
					{
						 pwd_handle(buf);
						 len_buf=strlen(buf);
						 send(new_fd,&len_buf,4,0);
						 send(new_fd,buf,len_buf,0);
					}
					else if(strcmp("exit",ord)==0)//客户端退出
					{
						printf("客户端退出 %d\n",new_fd);
						close(new_fd);
						break;
					}
					else{
						sprintf(buf,"输入有误，请核对后输入\n");
					}
				}
			}
		}else{
			printf("客户端密码错误,客户端请重连\n");
		}
		write(fdr,&overflg,4);//对主进程发送完毕信号
	}
}
