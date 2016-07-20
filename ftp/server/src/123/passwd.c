#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <shadow.h>
#include <errno.h>

void help(void)
{
	printf("用户密码验证程序\n 第一个参数为用户名！\n");
	exit(-1);
}

void error_quit(char *msg)
{
	perror(msg);
	exit(-2);
}

void get_salt(char *salt,char *passwd)
{
	int i,j;
	for(i=0,j=0;passwd[i]&&j!=3;i++)//取出salt,i记录密码字符下标，j记录$出现次数
	{
		if(passwd[i]=='$')
		{
			j++;
		}
	}
	strncpy(salt,passwd,i-1);
}

int main()
{
	struct spwd *sp;
	char *passwd;
	char salt[512]={0};
	passwd="vaio";//输入用户密码
	if((sp=getspnam("vaio"))==NULL)//得到用户名以及密码，命令行参数的第一个参数
	{
		printf("没有权限，请将服务器切换到root\n");
		return -1;
	}
	get_salt(salt,sp->sp_pwdp);//得到salt，用得到的密码作参数
	
	if(strcmp(sp->sp_pwdp,(char*)crypt(passwd,salt))==0)//进行密码验证
	{
		printf("验证成功!\n");
	}else{
		printf("验证失败!\n");
	}
	return 0;
}
