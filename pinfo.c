#include "headers.h"
#include "pinfo.h"

int pinfo(int pid)
{
	
	char * buff = (char *)malloc(1024*sizeof(char));
	char * buf2 = (char *)malloc(1024*sizeof(char));
        sprintf(buff,"/proc/%d/stat",pid);
	//printf("file name is %s\n",buff);
	FILE* file =  fopen(buff ,"r");
	if (file==NULL)
    	{
     	   fprintf(stderr,"Invalid PID!!\n");
     	   return -1;
    	}
	int ppid;
	char status;

	fscanf(file,"%d %*s %c",&ppid, &status);

	printf("pid -- %d\n",ppid);

	printf("Process Status -- %c\n",status);
	memset(buff,'\0',1023);
	int memory;
	long long int waste;
	for(int i=0;i<19;i++)
	{
		fscanf(file,"%lld",&waste);
	}
	fscanf(file, "%d",&memory);
	printf("Memory -- %d\n",memory);
	memset(buff,'\0',1023);
	sprintf(buff,"/proc/%d/exe",pid);
	int link=readlink(buff, buf2,1023);
	if(link==-1)
	{
		perror(buff);
		return -1;
	}
	buf2[link]='\0';
	int pppppid = getpid();
	if(pppppid == pid)
	{
		int i;
		for( i= strlen(buf2)-1 ; i>=0;i--)
		{
			if(buf2[i]=='/')
			{
				break;
			}
		}
		printf("Executable Path -- ~");
		for( i = i + 0;i<strlen(buf2);i++)
		{
			printf("%c",buf2[i]);
		}
		printf("\n");
		return 0;
	}
	printf("Executable Path -- %s\n",buf2);
	return 0 ;
	//	printf("%s\n",buf2);
	
}
