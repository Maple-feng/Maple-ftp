#include "handle.h"

int cd_handle(char *s,char *buf)
{	
	int ret;
	ret=chdir(s);
	char *p="chdir: No such file or directory";
	if(-1==ret)
	{
		sprintf(buf,"%s",p);
	}
	return ret;
}

int remove_handle(char *buf,char *filename)
{
	int ret=remove(filename);
	char *p="chdir: No such file or directory";
	if(-1==ret)
	{
		sprintf(buf,"%s",p);
	}
	return ret;
}

int pwd_handle(char *buf)
{
	getcwd(buf,2000);
	return 0;
}
