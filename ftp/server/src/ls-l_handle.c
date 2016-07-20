#include "handle.h"

int lsl_handle(char *buf1)
{
	DIR *dir;
	dir=opendir("./");
	if(NULL==dir)
	{
		perror("opendir");
		return -1;
	}
	struct dirent *p;
	struct stat buf;
	char s[100];
	char time[100];
	char buf2[100];
	char u_t[100];
	char g_t[100];
	char mode[11];
	int ret;
	int i=0;
	while((p=readdir(dir))!=NULL)
	{
		memset(s,0,sizeof(s));
		memset(&buf2,0,sizeof(buf2));
		memset(mode,0,sizeof(mode));
		sprintf(s,"%s/%s","./",p->d_name);
		ret=stat(s,&buf);
		if(ret==-1)
		{
			perror("stat");
			return -1;
		}
		if(p->d_name[0]!='.')
		{
			memset(time,0,sizeof(time));
			time_show(time,&buf.st_mtime);
			uid_name(buf.st_uid,u_t);
			gid_name(buf.st_gid,g_t);
			mode_show(buf.st_mode,mode);
			sprintf(buf2,"%s %d %s %s %5ld %s %s\n",mode,buf.st_nlink,u_t,g_t,buf.st_size,time,p->d_name);
			strcat(buf1,buf2);
		}
	}
	return 0;
}
