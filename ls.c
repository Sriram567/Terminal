#include "headers.h"
#include "ls.h"
#include "permissions.h"

int ford(char * argument)
{
	struct stat finder;

	stat(argument, &finder);

	if(S_ISREG(finder.st_mode))
	{
		return 0;
	}
	return 1;
}
void type(struct stat fstat)
{
	if(S_IFDIR & (fstat.st_mode))
	{
		printf("d");
	}
	else if(S_IFREG & (fstat.st_mode))
	{
		printf("-");
	}
	else
	{
		printf("l");
	}
}
void dt(struct stat filestat)
{
	char buffer [80];
	struct tm * timeinfo;
	time_t timeStamp = filestat.st_mtime;
	timeinfo = localtime (&timeStamp);
	strftime (buffer,80," %h  %e   %I:%M   ",timeinfo);
	printf("\t%s",buffer);
}
void stuff(struct stat filestat)
{
	printf("  %ld", filestat.st_nlink);
	printf("\t%s", getpwuid(filestat.st_uid)->pw_name);
	printf("\t%s", getgrgid(filestat.st_gid)->gr_name);
	printf("%11ld", filestat.st_size);
}
void printnamedir(struct dirent * dr, struct stat fstat)
{
	 if(S_ISDIR(fstat.st_mode))
        {
   		printf("%s\n" , dr->d_name);     
	}
        else if(S_ISREG(fstat.st_mode))
        {
        	printf("%s\n" , dr->d_name);
        }
        else
        {
                char * buff = (char *)malloc(1024*sizeof(char));

		int link=readlink(dr->d_name, buff,1023);
        	if(link==-1)
       		{
	                perror(buff);
	                return;
	        }
	        buff[link]='\0';
		printf("%s\n",buff);

        }
}
void printnamefile(char *arg)
{
	printf("%s\n",arg);
}
void both(char * argument ,struct dirent * readfolder)
{
	struct stat filestat;
	char buff[10240];
	sprintf(buff , "%s/%s", argument,readfolder->d_name);
	stat(buff, &filestat);
	type(filestat);
	permissions(filestat);
	stuff(filestat);
	dt(filestat);
	printnamedir(readfolder,filestat);
	//                      printf("\n");
}
void l(char * argument ,struct dirent * readfolder)
{
	struct stat filestat;
	char buff[10240];

	if(readfolder->d_name[0] !='.')
	{
		sprintf(buff , "%s/%s", argument,readfolder->d_name);
		stat(buff, &filestat);
		type(filestat);
		permissions(filestat);
		stuff(filestat);
		dt(filestat);
		printnamedir(readfolder,filestat);
//	               printf("\n");
	}
}
void a(char * argument,struct dirent * readfolder)
{
	struct stat filestat;
        char buff[10240];
        sprintf(buff , "%s/%s", argument,readfolder->d_name);
        stat(buff, &filestat);
	printnamedir(readfolder,filestat);

}

void normal(char * argument,struct dirent * readfolder)
{
	struct stat filestat;
        char buff[10240];
        sprintf(buff , "%s/%s", argument,readfolder->d_name);
        stat(buff, &filestat);
	if(readfolder->d_name[0] !='.')
	{
		printnamedir(readfolder,filestat);
	}
}
	
int mainls(int i,int j,char * argument)
{
	if(ford(argument))
	{
		DIR *folder;
		struct dirent* readfolder;
		folder = opendir(argument);
		if(folder == NULL)
		{
			fprintf(stderr,"ls : file doesnot exists\n");
			return -1;
		}
		readfolder = readdir(folder);
		while(readfolder!=NULL)
		{
			if(i & j)
			{
				both(argument ,readfolder);
			}
			else if(i)
			{
				l(argument ,readfolder);
			}
			else if(j)
			{
				a(argument,readfolder);
			}
			else
			{
				normal(argument,readfolder);
			}
			readfolder = readdir(folder);
		}
		closedir(folder);
	}
	else
	{
		fprintf(stderr,"ls : DIRECTORY SHOULD BE GIVEN AS ARGUMENT!!\n");
		return -1;
	}
	return 0;

}
int ls(int k,char ** parsed, char * scwd)
{
	int opt,il=0,ja=0;

	int j=0,givenarg=0, numarg = 0;;

	char *sep[1005],*argument[1024];
	for(int i=1;i<k;i++)
	{
		//printf("i = %d\n",i);
		if(parsed[i][0]=='-')
		{
			parsed[i][strlen(parsed[i])] = '\0';
			sep[j] = parsed[i];
			//			printf("%s\n",sep[j]);
			j++;
		}
		else
		{
			//			printf("en1\n");
			argument[numarg] = parsed[i];
			//			printf("en2\n");
			numarg++;
			givenarg = 1;

		}
	}
	//	printf("seperated\n");
	for(int ppp=0;ppp<j;ppp++)
	{
		for(int i=1;i<strlen(sep[ppp]);i++)
		{
			if(sep[ppp][i] == 'l')
			{
				il =1;
			}
			else if(sep[ppp][i] == 'a')
			{
				ja = 1;
			}
			else
			{
				fprintf(stderr,"INVALID OPTION '%c' is given\n",sep[ppp][i]);
				return -1;
			}
		}
	}
	//	printf(" l is there : %d\n a is there : %d\n",il,ja);
	if(givenarg == 0)
	{
		char cwd[1024];
		getcwd(cwd, sizeof(cwd));
		printf(".:\n");
		mainls(il,ja,cwd);

	}
	else
	{
	
		for(int i=0;i<numarg;i++)
		{

			if(argument[i][0]== '~')
			{
				printf("~:\n");
				//      printf("enter\n");
				//      printf("exit\n");
				return (mainls(il,ja,scwd));
			}
			else
			{
				printf("%s:\n",argument[i]);
				return (mainls(il,ja,argument[i]));
			}

		}
	}

}
