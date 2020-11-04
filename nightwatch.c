#include "headers.h"
#include "nightwatch.h"
int getch()
{
	char ch;
	int iot;
	struct termios oldter;
	struct termios newter;
	tcgetattr(STDIN_FILENO, &oldter);
	newter = oldter; 
	newter.c_lflag = newter.c_lflag & ~(ICANON | ECHO); 
	tcsetattr(STDIN_FILENO, TCSANOW, &newter);
	ioctl(0, FIONREAD, &iot);
	if(iot>0)
	{
		ch = getchar();

	}
	//       printf("on the war???");	
	tcsetattr(STDIN_FILENO, TCSANOW, &oldter); 
	if(ch=='q')
		return 1;
	return 0; 
}

int timeout(int seconds)
{
	clock_t timeout;
	timeout = clock () + seconds * CLOCKS_PER_SEC ;
	//	printf("time out is %d\n",timeout);
	while (clock() < timeout)
	{

		if(getch())
		{
			return 0;
		}
		//	printf("............%d\n",clock());	
	}
	return 1;
}
int nightwatch1( int seconds)
{
	int vital =0;
	
	char * buff = (char *)malloc(1024*sizeof(char));
	//	char * buff2 = (char *)malloc(1024*sizeof(char));
	FILE* file =  fopen("/proc/interrupts" ,"r");

	if(file == NULL)
	{
		fprintf(stderr,"ERROR OCCURED!!");
		return -1;
	}

	for(int i=0;i<100;i++)
	{
		fgets(buff, 1024, file);
		if(i==0)
		{
			printf("%s",buff);
			//	strcpy(buff2, buff);
		}
		for(int j=0;j<strlen(buff);j++)
		{
			if(buff[j]!=' ')
			{
				if(buff[j]=='1')
				{
					vital = i;
					break;
				}
					
				
			}
		}
		if(vital >0)
		{
			break;
		}
	}
	
	printf("%s",buff);
	fclose(file);
	//	printf("seconds is %d\n",seconds);
	while(timeout(seconds))
	{
		file =  fopen("/proc/interrupts" ,"r");
		for(int i=0;i<vital;i++)
		{
			fgets(buff, 1024, file);
		}
		fgets(buff, 1024, file);
		printf("\r");
		printf("%s",buff);
		fclose(file);
	}

}
int nightwatch2(int seconds)
{
	char * buff = (char *)malloc(1024*sizeof(char));
	FILE* file;
	file =  fopen("/proc/loadavg" ,"r");
                if(file == NULL)
                {
                        fprintf(stderr,"ERROR OCCURED!!");
                        return -1;
                }

                fgets(buff, 1024, file);
                int cnt = 0;
                //                prrintf("%s\n",buff2);
                for(int i=0;i<strlen(buff);i++)
                {
                        if(cnt>=4)
                        {
                                printf("%c",buff[i]);
                        }
                        if(buff[i]== ' ')
                        {
                                cnt++;
                        }
                }
                fclose(file);

	while(timeout(seconds))
	{
		file =  fopen("/proc/loadavg" ,"r");
		if(file == NULL)
		{
			fprintf(stderr,"ERROR OCCURED!!");
			return -1;
		}

		fgets(buff, 1024, file);
		int cnt = 0;
		//                prrintf("%s\n",buff2);
		for(int i=0;i<strlen(buff);i++)
		{
			if(cnt>=4)
			{
				printf("%c",buff[i]);
			}
			if(buff[i]== ' ')
			{
				cnt++;
			}
		}
		fclose(file);
	}
	//        fclose(file);
}



