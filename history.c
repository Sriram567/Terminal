#include "headers.h"
#include "history.h"
int fd_write;
FILE * ffwrite;
char * history[1000];
int count =0;
char scwd[1024];
void create_history(char * sd)
{
	strcpy(scwd, sd);
        int length;
        char ch[300];
        size_t size = 1024;
        fd_write = open("HiStO-rY.",O_RDONLY |O_APPEND| O_WRONLY | O_CREAT , 0777);
	if(fd_write==-1)
	{
		fprintf(stderr,"Cannot access history file\n");
		return ;
	}

        ffwrite = fopen("HiStO-rY.","a+");
//      printf("step one\n");
        while( fgets(ch,300,ffwrite) )
        {
                history[count] =(char *)malloc(200*(sizeof(char)));
                strcpy(history[count],ch);
//              printf("ch is %s   history is  %s\n",ch,history[count]);
                count ++;
        }
	fclose(ffwrite);
	
//      printf("COUNT is %d\n",count);
}
void addhistory(char * buff)
{
//	printf("adding to history]\n");
        if(strcmp(buff,"\n")!=0){
        int l = strlen(buff);
        buff[strlen(buff)] = '\0';

        if(count == 0)
        {
                history[count] =(char *)malloc(200*(sizeof(char)));
                strcpy(history[count],buff);
                write(fd_write,buff,l);
                count++;
                return ;
        }
        if(strcmp(buff,history[count-1]) != 0)
        {
        history[count] =(char *)malloc(200*(sizeof(char)));
        strcpy(history[count],buff);
        write(fd_write,buff,l);
        count++;
        }
        if(count > 20)
        {
                //printf("fine fine\n");
                for(int i=0;i<20;i++)
                {
                        strcpy(history[i],history[i+1]);
                }
                
                close(fd_write);
		char hisss[1040];
		sprintf(hisss,"%s/HiStO-rY.",scwd);
                fd_write = open(hisss,O_RDONLY |O_APPEND| O_WRONLY | O_CREAT |O_TRUNC , 0777);
                for(int i=0;i<20;i++)
                {
                        write(fd_write,history[i],strlen(history[i]));
                }
                count = 20;
        }
//      printf("newly added element is %s\n",history[count-1]);
//      printf("COUNT is %d\n",count);
        }
	
//	printf("einded history\n");
}
int gethistory(int number)
{
//      printf("NUMBER IS %d\n",number);
        if(number == -1)
        {
		int minn =0;
		if(count > 10)
		{
			minn = count - 10;
		}
                for(int i=minn;i<count;i++)
                {
                        printf("%s",history[i]);
                }
        }
        else
        {
                int minn = 0;
                if(number > 20)
                {
                        number = 20;
                }
                if(count-number>0)
                {
                        minn = count - number;
                }
                for(int i=minn;i<count;i++)
                {
                        printf("%s",history[i]);
                }
        }
	return 0;
}

