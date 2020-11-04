#include "headers.h"
#include "ls.h"
#include "pinfo.h"
#include "history.h"
#include "nightwatch.h"
extern int errno;
int changed = 0;
int fdin;
int fdout,coo=0;
char falsecommand[1024];
int error = 0;
int foregroundprocess = -1;
char scwd[1000];
char prevdir[1024];
struct bgprocess
{
	int flag;
	int pid;
	char  tokens[1024];
	//	char *dtokens[1024];
	struct bgprocess *prev;
	struct bgprocess *next;
};
struct bgprocess *head = NULL;
/*void nochange(int signum)
  {
  printf("done !!!\n");
  if(foregroundprocess!=-1)
  {
  kill(foregroundprocess, SIGTSTP);
  }
  signal(SIGTSTP,nochange);

  }*/
int Gotopipe(int k , char ** parsed);
void quit()
{
	struct bgprocess *node = head;
	while(node!= NULL)
	{
		kill(node->pid,SIGKILL);
		node = node->next;
	}
	printf("\n");
	exit(0);


}
bool typedany(char * str)
{
	dup2(fdin,STDIN_FILENO);
	        dup2(fdout,STDOUT_FILENO);

	int ss;
	//waitpid(6957,&ss,WUNTRACED);
	foregroundprocess = -1;
	//	signal(SIGTSTP,SIG_IGN);
	/*	if(coo)
		{
			printf("*****************************************************\n");
		}
		*/
	error = 0;
	char * buffer = (char *)malloc(10024*sizeof(char));

	size_t size = 1024;
	//printf("...\n");
	int ret = getline(&buffer, &size , stdin);
	if(ret<0)
	{
		quit();
	}
	//printf("top right!!\n");
	if(buffer==NULL)
	{
		return 0;
	}
	if(strlen(buffer))
	{
		strcpy(str , buffer);
		return true;
	}
	else
	{
		return false;
	}
}
int jobs()
{
	int cntt = 1;
	struct bgprocess * node = head;
	if(true)
	{

	}
	while(node != NULL)
	{
		char * buff = (char *)malloc(1024*sizeof(char));
		char * buf2 = (char *)malloc(1024*sizeof(char));
		sprintf(buff,"/proc/%d/stat",node->pid);
		//printf("file name is %s\n",buff);
		FILE* file =  fopen(buff ,"r");
		if (file==NULL)
		{
			node= node->next;
		}
		int ppid;
		char status;
		int pgid;
		fscanf(file,"%d %*s %c %*d %d",&ppid, &status,&pgid);

		if(status == 'R')
		{

			printf("[%d] Running ",cntt);
			int notime = 0;
			while(node->tokens[notime]!='\n' && node->tokens[notime]!='&')
			{
				printf("%c",node->tokens[notime]);
				notime++;
			}

			/*	while( strlen(node->dtokens[notime]) )
				{
				printf("%s ",node->dtokens[notime]);
				notime++;
				}*/
			printf(" [%d]\n",ppid);
		}
		else if(status == 'S')
		{
			printf("[%d] Sleeping ",cntt);
			int notime = 0;
			while(node->tokens[notime]!='\n' && node->tokens[notime]!='&')
			{
				printf("%c",node->tokens[notime]);
				notime++;
			}
			/*	while(node->dtokens[notime]!=NULL)
				{
				printf("%s ",node->dtokens[notime]);
				notime++;
				}
				*/
			printf(" [%d]\n",ppid);

		}
		else if(status == 'D')
		{
			printf("[%d] Waiting ",cntt);
			int notime = 0;
			/*	while(strlen(node->dtokens[notime]))
				{
				printf("%s ",node->dtokens[notime]);
				notime++;
				}
				*/
			while(node->tokens[notime]!='\n' && node->tokens[notime]!='&')
			{
				printf("%c",node->tokens[notime]);
				notime++;
			}

			printf(" [%d]\n",ppid);

		}
		else if(status == 'T')
		{

			printf("[%d] Stopped ",cntt);
			int notime = 0;
			/*while(strlen(node->dtokens[notime]))
			  {
			  printf("notime is %d\n",notime);
			  printf("%s ",node->dtokens[notime]);
			  notime++;
			  }
			  */
			while(node->tokens[notime]!='\n'&& node->tokens[notime]!='&')
			{
				printf("%c",node->tokens[notime]);
				notime++;
			}

			printf(" [%d]\n",ppid);

		}
		node = node->next;
		cntt++;
	}
	return 0;
}
void printline(char * scwd)
{
	char host[1024],cwd[1000],command[1024],cwd2[1000];
	char *user, *tok;
	int hostNum;

	user = getenv("USER");
	hostNum = gethostname(host,sizeof(host));

	getcwd(cwd,sizeof(cwd));
	//	printf("cwd is %s\n",cwd);
	for(int i=0;i<strlen(scwd);i++)
	{
		cwd2[i]= cwd[i];
	}
	cwd2[strlen(scwd)] = '\0';

	if(strcmp(scwd, cwd2)==0)
	{

		for(int i=strlen(scwd);i<strlen(cwd);i++)
		{
			cwd2[i-strlen(scwd)] = cwd[i];
		}
		cwd2[strlen(cwd)-strlen(scwd)] = '\0';	

		printf("%s@%s: ~%s >",user,host,cwd2);
		fflush(stdout);
	}
	else
	{
		printf("%s@%s: %s >",user,host,cwd);
		fflush(stdout);
	}
}
int echo(int k , char ** parsed)
{
	if(k==1)
	{
		printf("FOR ECHO INPUT HAS TO BE PROVIDED!!\n");
		return 0;
	}

	char str[10004]="";

	for(int i=1;i<k-1;i++)
	{
		strcat(str,parsed[i]);
		strcat(str," ");
	}
	strcat(str,parsed[k-1]);
	int len = strlen(str);

	write(1,str,len);
	write(1,"\n",1);
	return 0;

}

int call_ls(int k,char ** parsed,char * scwd)
{
	return (ls(k,parsed,scwd));
}
int other(int k, char ** parsed);
int execcommand(int k , char ** parsed);
int checkre2(char * command)
{
//	printf("entered\n");
	char *pipeparsed[1024];
	char sparecommand[1024];
        strcpy(sparecommand,command);

        char *parsed[1005],*tok;

        //printf("the command is %s",command);
        tok = strtok(command," \r\n\t");
        int k=0;
        while(tok != NULL)
        {
                parsed[k] = tok;
                //      printf("parsed is%s\n",parsed[k]);
                k++;
                tok = strtok (NULL," \r\n\t");
        }

        //printf("parsed o is %s",parsed[0]);
        if(k==0)
        {
                
                return 1;
        }
	int a = 0;
	int retval = 1,no_args=0;

	for(int i=0;i<k;i++)
	{
		if(strcmp(parsed[i],"@")==0)
		{
			pipeparsed[no_args]= (char *)malloc(1024*sizeof(char));
                        pipeparsed[no_args] = NULL;
//			printf("gone into\n");
			int F = Gotopipe(no_args,pipeparsed);
	//		printf("F is %d\n",F);
			if(F==0)
			{
				retval = 1;
			}
			else 
			{
				retval = -1;
			}

			a = 0;
			no_args= 0;
			if(retval==-1)
			{
	//			printf("entered\n");
			while(i<k && (strcmp(parsed[i],"$")!=0))
			{
//				printf("..\n");
				i++;
			}
			if(i<k)
			{
				a=1;
			}

			}
		}
		else if(strcmp(parsed[i],"$")==0)
                {
			pipeparsed[no_args]= (char *)malloc(1024*sizeof(char));
                        pipeparsed[no_args] = NULL;

                        int F = Gotopipe(no_args,pipeparsed);

                        if(F==0)
                        {
                                retval = 1;
                        }
                        else
                        {
                                retval = -1;
                        }

                        a = 1;
                        no_args= 0;
			if(retval==1){
				
                        while(i<k && (strcmp(parsed[i],"@")!=0))
                        {
                                i++;
                        }
                        if(i<k)
                        {
                                a=0;
                        }}
                        
                }
		else
		{
			 pipeparsed[no_args]= (char *)malloc(1024*sizeof(char));
	                 pipeparsed[no_args] = parsed[i];
			 no_args++;

		}
		

	}

	if(no_args>0)
	{
	//	printf("it is here\n");
		 pipeparsed[no_args]= (char *)malloc(1024*sizeof(char));
                 pipeparsed[no_args] = NULL;

                        int F = Gotopipe(no_args,pipeparsed);

                        if(F==0)
                        {
                                retval = 1;
                        }
                        else
                        {
                                retval = -1;
                        }

                        a = 1;
                        no_args= 0;
	}
	if(retval<0)
		return -1;
	return 0;

}
int checkre(int no_args , char ** parsed)
{
	int state=-1;
	int in =0 , out = 0, moreout = 0;
	char * filenamein;
	char * filenameout;
	for(int i=0;i<no_args;i++)
	{
		//		printf("otherloop\n");
		if(strcmp(parsed[i],"<")==0)
		{
			if(parsed[i+1]!=NULL)
			{
				filenamein = (char *)malloc(1024*sizeof(char));
				filenamein = parsed[i+1];
				in = 1;
			}
		}
		else if(strcmp(parsed[i],">")==0)
		{

			if(parsed[i+1]!=NULL)
			{
				filenameout = (char *)malloc(1024*sizeof(char));
				//sprintf(filenameout, "%s",parsed[i+1]);
				//filenameout[strlen(filenameout)]='\0';
				filenameout = parsed[i+1];
				out = 1;
			}
		}
		else if (strcmp(parsed[i],">>")==0)
		{
			filenameout = (char *)malloc(1024*sizeof(char));
			//sprintf(filenameout, "%s",parsed[i+1]);
			//filenameout[strlen(filenameout)]='\0';
			filenameout = parsed[i+1];
			out = 1;
			moreout = 1;
		}

	}
	char * finalparsed[1024];
	int finalcnt = 0;
	for(int i=0;i<no_args;i++)
	{
		if(strcmp(parsed[i],"<") == 0 || strcmp(parsed[i],">") == 0 || strcmp(parsed[i],">>")==0)
		{
			i++;
			continue;
		}
		finalparsed[finalcnt] = parsed[i];
		finalcnt++;
	}
	finalparsed[finalcnt] = (char *)malloc(1024*sizeof(char));
	finalparsed[finalcnt] = NULL;
	//	printf("sucess\n");
	if(!in && !out)
	{
		int jj = (execcommand(no_args,parsed));
		return jj;
	}
	else
	{
		//	printf("ENTERED\n");
		if(in && out)
		{
			int fdread,fdwrite;
			if((fdread = open(filenamein, O_RDONLY, 0644)) < 0)
			{
				perror("open error");
				return -1;
			}
			if(moreout)
			{
				if((fdwrite = open(filenameout, O_WRONLY |O_APPEND | O_CREAT, 0644)) < 0)
				{
					perror("open error");
					return -1;
				}
			}
			else
			{
				if((fdwrite = open(filenameout, O_WRONLY | O_CREAT |O_TRUNC, 0644)) < 0)
				{
					perror("open error");
					return -1;
				}

			}
			dup2(fdread, STDIN_FILENO);
			dup2(fdwrite, STDOUT_FILENO);
			int jjjj= (execcommand(finalcnt, finalparsed));
			close(fdread);
			close(fdwrite);
			return jjjj;
			/*if (execvp(finalparsed[0], finalparsed) < 0) {
			//error =1;
			printf("\nCould not execute command 1..\n");
			exit(0);
			}*/

		}
		else if(in)
		{
			//			printf("entered in++++++++++++++++++++++++++++++++++++++++++++++++\n");
			int fdread,fdwrite;
			if((fdread = open(filenamein, O_RDONLY, 0644)) < 0)
			{
				perror("open error");
				return -1;
			}

			dup2(fdread, STDIN_FILENO);
			int jj = execcommand(finalcnt, finalparsed);
			/*if (execvp(finalparsed[0], finalparsed) < 0) {
			//error =1;
			printf("\nCould not execute command 1..\n");
			exit(0);
			}*/
			close(fdread);
			return jj;
		}
		else if(out)
		{
			//			                        printf("entered out++++++++++++++++++++++++++++++++++++++++++++++++\n");

			int fdread,fdwrite;
			if(moreout)
			{
				if((fdwrite = open(filenameout, O_WRONLY |O_APPEND | O_CREAT, 0644)) < 0)
				{
					perror("open error");
					return -1;
				}
			}
			else
			{
				if((fdwrite = open(filenameout, O_WRONLY | O_CREAT |O_TRUNC, 0644)) < 0)
				{
					perror("open error");
					return -1;
				}

			}
			dup2(fdwrite, STDOUT_FILENO);
			close(fdwrite);
			return (execcommand(finalcnt, finalparsed));
			/*if (execvp(finalparsed[0], finalparsed) < 0) {
			//error =1;
			printf("\nCould not execute command 1..\n");
			exit(0);
			}*/

		}
		return 0;
	}
}
int Gotopipe(int k, char ** parsed)
{
//	printf("received\n");
	//	printf("gotopipe\n");
	int j = 0;
	int cnt = 0;
	char *pipeparsed[1024];
	int pipefd[2];
	int p1,p2;


	if (pipe(pipefd) < 0) 
	{
		fprintf(stderr,"\nPipe could not be initialized\n");
		return -1;
	} 
	fdin = dup(STDIN_FILENO);
	fdout = dup(STDOUT_FILENO);


	/*	dup2(pipefd[0], STDIN_FILENO); 
		dup2(pipefd[1], STDOUT_FILENO);*/

	int no_args = 0;
	//	printf("k is %d\n",k);
	for(int kk = 0;kk < k;kk++)
	{
		//		printf("loop\n");
		if(strcmp(parsed[kk],"|")==0)
		{

			if(true)
			{

				pipeparsed[no_args]= (char *)malloc(1024*sizeof(char));
				pipeparsed[no_args] = NULL;

				if (pipe(pipefd) < 0)
				{
					fprintf(stderr,"\nPipe could not be initialized\n");
					return -1;
				}
				dup2(pipefd[1], STDOUT_FILENO);
				//	checkre(no_args, pipeparsed);

				int returnval = checkre(no_args,pipeparsed);	
				dup2(pipefd[0], STDIN_FILENO);
				close(pipefd[1]);
				close(pipefd[0]);	
				/*int freturn1 = fork();


				  if(freturn1 == 0)
				  {
				  int re =  checkre(no_args, pipeparsed);
				  if(re==-1)
				  {
				//			       printf("entered -1 \n");
				if (execvp(pipeparsed[0], pipeparsed) < 0) {
				error =1;
				printf("Could not execute command 1..\n");
				exit(0);
				}
				}
				else if(re == 0)
				{
				printf("error occured!!\n");
				}
				}
				else
				{
				int status;
				waitpid(freturn1, &status , WUNTRACED);
				dup2(pipefd[0], STDIN_FILENO);
				close(pipefd[1]);
				close(pipefd[0]);
				}*/
			}

			no_args = 0;
		}
		else
		{
			pipeparsed[no_args]= (char *)malloc(1024*sizeof(char));
			//		printf("yes2\n");
			pipeparsed[no_args] = parsed[kk];

			no_args++;
		}
		if(error)
		{
			fprintf(stderr,"pipe failed\n");
			dup2(fdin, STDIN_FILENO);
			dup2(fdout, STDOUT_FILENO);
			
			return -1;
		}
	}
	dup2(fdout, STDOUT_FILENO);
	//		printf("completed\n");
	//	printf("pipeparsed is %s and args is %d\n",pipeparsed[no_args-1], no_args);
	pipeparsed[no_args]= (char *)malloc(1024*sizeof(char));
	pipeparsed[no_args] = NULL;
	/*	int freturn12 = fork();
		if(freturn12 == 0)
		{
		int re =  checkre(no_args, pipeparsed);
		if(re==-1)
		{
	//                             printf("entered -1 \n");
	if (execvp(pipeparsed[0], pipeparsed) < 0) {
	error =1;
	printf("\nCould not execute command 1..\n");
	exit(0);
	}
	}
	else if(re == 0)
	{
	printf("error occured!!\n");
	}

	}
	else
	{
	//			printf("hogaya2\n");
	int status;
	waitpid(freturn12, &status , WUNTRACED);
	}*/
	//	printf("pipes done!!!\n");

	int returnval = checkre(no_args,pipeparsed);
	if(error)
	{
		fprintf(stderr,"pipe failed\n");
		dup2(fdin, STDIN_FILENO);
		dup2(fdout, STDOUT_FILENO);
		return -1;
	}
	dup2(fdin, STDIN_FILENO);
	dup2(fdout, STDOUT_FILENO);
//	printf("retval is %d\n",returnval);
	if(returnval == -1)
	{
		return -1;
		
	}
	else
	{
		return 0;
	}
}
int retcnt()
{
	struct bgprocess * node = head;
	int cnt=0;
	while(node!=NULL)
	{
		node= node->next;
		cnt++;
	}
	return cnt;

}
int fg(int k , int num)
{
	if(!num)
	{
		fprintf(stderr,"Invalid arguments are given\n");
		return -1;
	}
	if(k<2 || k>2)
	{
		fprintf(stderr,"CORRECT NUMBERS OF ARGUMENTS ARE TO BE GIVEN!!!\n");
		return 0 ;
	}
	struct bgprocess * node = head;
	int cnttt = retcnt();
	if(cnttt < num)
	{
		fprintf(stderr,"THE JOB NUMBER DOESN'T EXIST!!!!\n");
		return 0;
	}
	for(int ji=1;ji<num;ji++)
	{
		node= node->next;
	}
	//		printf("send to pid %d\n\n",node->pid);
	//	int ret = kill(node->pid, SIGTSTP);
	node->flag = 1;
	/*if(ret<0)
	  {
	  perror("KILL :- ");
	  }*/
	int ret;
	int notime = 0;
/*	while(node->tokens[notime]!='\0' && node->tokens[notime]!='\n' && node->tokens[notime]!='&')
	{
		printf("%c",node->tokens[notime]);
		notime++;
	}
	printf("\n");*/

	if(getpgid(0)== getpgid(node->pid))
	{		

		//		ret = kill(node->pid, SIGCONT);
		ret = kill(node->pid, SIGCONT);
		node->flag = 1;
		if(tcsetpgrp(2,getpgid(node->pid)) == -1)
		{fprintf(stderr,"ERROR OCCURED!!!\n");perror("kkk");return -1 ;}
		signal(SIGTTOU,SIG_DFL);
		if(ret<0)
		{
			perror("donga");
		}

		//foregoundprocess = node->pid;
		if(ret<0)
		{
			perror("KILL :- ");
			return -1;
			//printf(":'(");
		}

		int status;
		if(waitpid(node->pid,&status,WUNTRACED)<0)
		{
			perror("waitpid");
			return -1;
		}
		signal(SIGTTOU,SIG_IGN);
		if(tcsetpgrp(2,getpid())==-1)
		{
			perror("tcsetpgrp is error");
			return -1;
		//	printf(":'(");

		}
		signal(SIGTTOU,SIG_DFL);
		if(WIFEXITED(status) != 0)
		{
			int notime = 0;
			while(node->tokens[notime]!='\0' && node->tokens[notime]!='\n' && node->tokens[notime]!='&')
                        {
                                fprintf(stderr,"%c",node->tokens[notime]);
                                notime++;
                        }
			fprintf(stderr, "with pid %d exited normally\n",node->pid);
			//printf(":')");
		}
		else if(WIFSTOPPED(status))
		{
			int rt = retcnt();

			fprintf(stderr,"[%d] Stopped ",rt);

			int notime = 0;
			while(node->tokens[notime]!='\0' && node->tokens[notime]!='\n' && node->tokens[notime]!='&')
			{
				fprintf(stderr,"%c",node->tokens[notime]);
				notime++;
			}
			printf("\n");
			return -1;
			//printf(":'(");


		}
		else if(WIFSIGNALED(status))
                {
				return -1;
               }


		//              signal(SIGTTOU,SIG_IGN);
		//

		return 0;

	}
	else
	{

		signal(SIGTTOU,SIG_IGN);



		//		ret = kill(node->pid, SIGCONT);
		ret = kill(node->pid, SIGCONT);
		node->flag = 1;
		if(tcsetpgrp(2,node->pid) == -1)
		{fprintf(stderr,"ERROR OCCURED!!!\n");
			perror("tcset");return -1 ;}
		signal(SIGTTOU,SIG_DFL);
		if(ret<0)
		{
			perror("donga");

		}

		//foregoundprocess = node->pid;
		if(ret<0)
		{
			perror("KILL :- ");
		}

		int status;
		if(waitpid(node->pid,&status,WUNTRACED)<0)
		{
			perror("waitpid");
			return -1;
		}
		signal(SIGTTOU,SIG_IGN);
		if(tcsetpgrp(2,getpid())==-1)
		{
			perror("tcsetpgrp is error");
		}
		signal(SIGTTOU,SIG_DFL);
		if(WIFEXITED(status) != 0)
		{
			  int notime = 0;
			  printf("\n");
                        while(node->tokens[notime]!='\0' && node->tokens[notime]!='\n' && node->tokens[notime]!='&')
                        {
                                fprintf(stderr,"%c",node->tokens[notime]);
                                notime++;
                        }
                        fprintf(stderr, "with pid %d exited normally\n",node->pid);
                        //printf(":')");


		}
		else if(WIFSTOPPED(status))
		{
			int rt = retcnt();

			fprintf(stderr,"[%d] Stopped ",rt);
			int notime = 0;
			while(node->tokens[notime]!='\0' && node->tokens[notime]!='\n' && node->tokens[notime]!='&')
			{
				fprintf(stderr,"%c",node->tokens[notime]);
				notime++;
			}
			printf("\n");
			return -1;
			//printf(":'(");

		}
		else if(WIFSIGNALED(status))
                        {
				return -1;
                        }


		//              signal(SIGTTOU,SIG_IGN);
		return 0;

	}

}
int execcommand(int k , char ** parsed)
{
	if((strcmp(parsed[0],"quit")==0))
	{
		quit();
	}
	else if(strcmp(parsed[0],"cd")==0)
	{
		//	printf("entered cd \n");

		//printf("parsed 1 is%s",parsed[1]);
		//	printf("changing to %s%d",parsed[1],strlen(parsed[1]));
		
		if(k>2)
        {
               fprintf(stderr,"Too Many Arguments!!!\n");
                return 0;
        }

		if(k==1)
		{

			getcwd(prevdir,sizeof(prevdir));
			chdir(scwd);
			//		printf("***********cd completed****************\n");

			return 0;
		}

		if(parsed[1][0]=='~')
		{
			getcwd(prevdir,sizeof(prevdir));
			//	printf("half");
			char sspare[1024];
       		        strcpy(sspare,prevdir);
                	getcwd(prevdir,sizeof(prevdir));
	//		printf("parsed is %s\n",parsed[1]+1);	
			char asdf[1024];
			strcpy(asdf,scwd);
			int check = chdir(strcat(asdf,parsed[1]+1));
	                changed = 1;
        	        if(check!=0)
                	{
                        	strcpy(prevdir,sspare);
                        	perror("cd");
				return -1;
			}

        //              printf(":'(");

			//			printf("***********cd completed****************\n");
			
				return 0;
			
		}

		if(parsed[1][0]=='-')
		{
			if(changed){
				printf("%s\n",prevdir);}
			char sspare[1024];
			strcpy(sspare,prevdir);

			getcwd(prevdir,sizeof(prevdir));
			chdir(sspare);
			return 0;
		}

		//	printf("jjj");
		char sspare[1024];
		strcpy(sspare,prevdir);
		getcwd(prevdir,sizeof(prevdir));
		int check = chdir(parsed[1]);
		changed = 1;
		if(check!=0)
		{
			strcpy(prevdir,sspare);
			perror("cd");

	//		printf(":'(");
			return -1;
		}
		else
		{

			//printf(":')");
			return 0;
		}
		//		printf("***********cd completed****************\n");
		return 0;
	}
	else if(strcmp(parsed[0],"fg")==0)
	{
		if(k<2 || k>2)
		{
			fprintf(stderr,"Arguments has to be provided\n");
			return -1;
		}

		for(int ii=0;ii<strlen(parsed[1]);ii++)
		{
	//		printf("%d\n",((int)(parsed[1][ii])));
			if(((int)(parsed[1][ii]))<48 || ((int)(parsed[1][ii]))>57)
			{
				fprintf(stderr,"String is not a valid argument!!\n");
				return -1;
			}
		}

		return (fg(k,atoi(parsed[1])));
	}
	else if(strcmp(parsed[0],"bg")==0)
	{
		if(k!=2)
		{
			fprintf(stderr,"CORRECT NUMBERS OF ARGUMENTS ARE TO BE GIVEN!!!\n");
			return -1 ;
		}
		struct bgprocess * node = head;
		int cnttt = retcnt();
		for(int ii=0;ii<strlen(parsed[1]);ii++)
                {
                        if(((int)(parsed[1][ii]))<48 || ((int)(parsed[1][ii]))>57)
                        {
                                fprintf(stderr,"String is not a valid argument!!\n");
                                return -1;
                        }
                }
		if(cnttt<atoi(parsed[1]) || atoi(parsed[1])<=0 )
                {
                        fprintf(stderr,"THE JOB NUMBER DOESN'T EXIST!!!!\n");
                        return -1;
                }


		for(int ji=1;ji<atoi(parsed[1]);ji++)
		{
			node= node->next;
		}
		node->flag = 0;
		//printf("send to pid %d\n\n",node->pid);
		int ret = kill(node->pid, SIGCONT);
		if(ret<0)
		{
			perror("KILL :- ");

	//		printf(":'(");
			return -1;
		}
		else{

		//	printf(":')");
		}
		return 0;
	}
	else if(strcmp(parsed[0],"kjob")==0)
	{
		if(k<3 || k>3)
		{
			fprintf(stderr,"CORRECT NUMBERS OF ARGUMENTS ARE TO BE GIVEN!!!\n");

	//		printf(":(");
			return -1;
		}

		struct bgprocess * node = head;
		int cnttt = retcnt();
		for(int ii=0;ii<strlen(parsed[1]);ii++)
                {
                        if(((int)(parsed[1][ii]))<48 || ((int)(parsed[1][ii]))>57)
                        {
                                fprintf(stderr,"kjob : String is not a valid argument!!\n");
                                return -1;
                        }
                }
                for(int ii=0;ii<strlen(parsed[2]);ii++)
                {
                        if(((int)(parsed[2][ii]))<48 || ((int)(parsed[2][ii]))>57)
                        {
                                fprintf(stderr,"String is not a valid argument!!\n");
                                return -1;
                        }
                }

		if(cnttt<atoi(parsed[1]) || atoi(parsed[1])<=0 )
		{
			fprintf(stderr,"THE JOB NUMBER DOESN'T EXIST!!!!\n");

		//	printf(":'(");
			return -1;
		}

		for(int ji=1;ji<atoi(parsed[1]);ji++)
		{
			node= node->next;
		}
		//printf("send to pid %d\n\n",node->pid);
		int ret = kill(node->pid, atoi(parsed[2]));
		if(ret<0)
		{
			perror("KILL :- ");

		//	printf(":'(");
			return -1;
		}

	//	printf(":')");
		return 0;
	}
	else if(strcmp(parsed[0],"overkill")==0)
	{
	//	fprintf(stderr,"entered overkill \n");
		if(k>1)
		{
			fprintf(stderr,"CORRECT NUMBERS OF ARGUMENTS ARE TO BE GIVEN!!!\n");
		}
		struct bgprocess *node = head;
		while(node!= NULL)
		{
			kill(node->pid,SIGKILL);
			node = node->next;
		}

	//	printf(":')");
		return 0;
	}
	else if(strcmp(parsed[0],"setenv")==0)
	{
		//		printf("*****************************entered*************************\n");
		if(k==1 || k>3)
		{
			dup2(fdout, STDOUT_FILENO);
			fprintf(stderr,"CORRECT NUMBER OF ARGUMENTS HAS TO BE GIVEN\n");
			return -1;

		}
		else if(k==2)
		{
			int re = setenv(parsed[1],"",1);
			if(re < 0)
			{
				dup2(fdout, STDOUT_FILENO);
				perror("setenv");
			//	printf(":'(");
				return -1;
			}
		}
		else if(k==3)
		{
			parsed[1][strlen(parsed[1])]='\0';
			parsed[2][strlen(parsed[2])]='\0';
			int re = setenv(parsed[1],parsed[2],1);
			if(re <0)
			{
				dup2(fdout, STDOUT_FILENO);
				perror("setenv");

			//	printf(":'(");
				return -1;
			}
		}

		return 0;
	}
	else if(strcmp(parsed[0],"jobs")==0)
	{
		if(k>1 || k<1)
		{
			fprintf(stderr,"INVALID NUMBER OF ARGUMENTS IS GIVEN!!\n");
			return -1;
		}
		jobs();
	}
	else if(strcmp(parsed[0],"unsetenv")==0)
	{
		if(k==1 || k>2)
		{
			dup2(fdout, STDOUT_FILENO);
			fprintf(stderr,"CORRECT NUMBER OF ARGUMENTS HAS TO BE GIVEN\n");
			//	printf(":'(");

			return -1;
		}
		else
		{
			unsetenv(parsed[1]);

			//		printf(":')");
			return 0 ;
		}
	}
	else if(strcmp(parsed[0],"echo") == 0)
	{
		echo(k , parsed);

		//		printf(":')");
		return 0;

	}
	else if(strcmp(parsed[0],"pwd") == 0)	
	{
		char cwd[1000];
		getcwd(cwd,sizeof(cwd));
		write(1,cwd,strlen(cwd));
		write(1,"\n",strlen("\n"));

		//	printf(":')");
		return 0;
	}


	else if(strcmp(parsed[0],"ls")==0)
	{
		//		printf("*****************************************enterd ls********************************************\n");
		int tt = call_ls(k,parsed,scwd);
		if(tt==-1)
		{

			//	printf(":'(");
			return -1;
		}
		else
		{

			//printf(":')");
			return 0;
		}
		return 0;
	} 
	else if(strcmp(parsed[0],"pinfo")==0)
	{
		if(k<2)
		{
			pinfo(getpid());
			return 0;
		}
		int tt;
		for(int i=1;i<k;i++)
		{

			tt = pinfo(atoi(parsed[i]));
			printf("\n");
		}
		if(tt==-1)
		{

			//printf(":'(");
			return -1;
		}
		else
		{

			//printf(":')");
			return 0;
		}

		return 0;
	}
	else if(strcmp(parsed[0],"history") == 0)
	{
		if(k==1)
		{
			gethistory(-1);
		}
		else
		{

			gethistory(atoi(parsed[1]));

		}
		return 0;
	}
	else if( strcmp(parsed[0],"nightswatch")==0 )
	{
		if(k<4)
		{
			fprintf(stderr,"arguments has to be provided\n");
			return 0;
		}

		if(strcmp(parsed[3],"interrupt")==0)
		{
			//   	printf("enter\n");
			nightwatch1(atoi(parsed[2]));
		}
		else if (strcmp(parsed[3],"newborn")==0)
		{
			nightwatch2(atoi(parsed[2]));
		}
		else
		{
			fprintf(stderr,"Arguments have to be corrected!!!\n");
		}
		return 0;
	}
	else{
	return(	other(k,parsed));
	}
	return 1;
}
void received(int signum)
{
	coo =1;
}
void func(int signum)
{
	int status;
	struct bgprocess *curr = head;

	while(curr!=NULL)
	{
		if( waitpid(curr->pid,&status, WNOHANG) )
		{
			if(WIFEXITED(status) != 0)
			{
				if(WEXITSTATUS(status) == 0)
				{
					if(!(curr->flag)){
						 int notime = 0;
                        while(curr->tokens[notime]!='\0' && curr->tokens[notime]!='\n' && curr->tokens[notime]!='&')
                        {
                                fprintf(stderr,"%c",curr->tokens[notime]);
                                notime++;
                        }


						fprintf(stderr," with pid %d exited normally\n",curr->pid);
						printf(":')");
						printline(scwd);}
				}
				else
				{
					if(!(curr->flag)){
						fprintf(stderr,"error : The Command given or the arguments provided is/are incorrect!!\n");
						//	perror(curr->tokens);
							printf(":'(");
						printline(scwd);}
				}

			}
			if(WIFSIGNALED(status))
			{
				if(!(curr->flag)){
					                        int notime = 0;
                        while(curr->tokens[notime]!='\0' && curr->tokens[notime]!='\n' && curr->tokens[notime]!='&')
                        {
                                fprintf(stderr,"%c",curr->tokens[notime]);
                                notime++;
                        }

					fprintf(stderr," with pid %d exited abnormally\n",curr->pid);
					printf(":'(");
					printline(scwd);}
			}

			if(curr->prev == NULL)
			{
				if(curr->next != NULL)
				{
					curr->next->prev = NULL;
				}
				head = curr->next;

			}
			else
			{
				//	fprintf(stderr, "hello right!!\n");
				curr->prev->next = curr->next;
				if(curr->next!=NULL)
				{
					curr->next->prev = curr->prev;
				}
				struct bgprocess * remove = curr;
				curr = curr->prev;
				//	fprintf(stderr, "hello right 2\n");
				free(remove);

			}
		}
		curr = curr->next;				
	}

	//	fprintf(stderr,"last error");

}
int other(int k , char **parsed)
{
	parsed[k] = NULL;
	int bg =0 ;	
	//	printf("parsed k-1 is %c",parsed[k-1][strlen(parsed[k-1])-1]);
	if(parsed[k-1][strlen(parsed[k-1])-1]=='&')
	{
		if(parsed[k-1][0]=='&')	
		{
			parsed[k-1]=NULL;
			bg =1;
		}
		else
		{
			parsed[k-1][strlen(parsed[k-1])-1] = '\0';
			bg =1;
		}
	}

	if(!bg)
	{
		int forkreturn = fork();
		if(forkreturn == -1)
		{
			fprintf(stderr,"UNABLE TO FORK!!!\n");

			//printf(":'(");
			return -1;
		}
		//		printf("foreground\n");
		if(forkreturn == 0)
		{
			setpgid(0,0);
			//	kill(getpid(),SIGTSTP);
			signal(SIGTTOU,SIG_IGN);
			if(tcsetpgrp(2,getpgid(0)) == -1)
			{fprintf(stderr,"ERROR OCCURED!!!\n");
				perror("ppp");
				return -1 ;}
			signal(SIGTTOU,SIG_DFL);


			//	signal(SIGTTOU,SIG_IGN);
			signal(SIGTSTP,SIG_DFL);
			signal(SIGINT,SIG_DFL);
			signal(SIGCONT,SIG_DFL);
			foregroundprocess = getpid();
			if (execvp(parsed[0], parsed) < 0)
			{
				dup2(fdout,STDOUT_FILENO);
				error =1;
				fprintf(stderr,"\n%s:Could not execute the command\n",parsed[0]);
				fflush(stdout);
				exit(-1);
				
				printf(":'(");
			}
		}
		else
		{
			setpgid(forkreturn, forkreturn);
			int status;


			waitpid(forkreturn, &status , WUNTRACED);
			
			signal(SIGTTOU,SIG_IGN);
			
			if(tcsetpgrp(2,getpgid(0)) == -1)
			{
				fprintf(stderr,"ERROR OCCURED!!!\n");
				return -1;
			}
			signal(SIGTTOU,SIG_DFL);

			if(WIFEXITED(status) != 0)
			{
//				printf("status is %d\n",WEXITSTATUS(status));
				if(WEXITSTATUS(status) != 0)
				return -1;
				else
				return 0;
				//printf(":')");
			}
			else if(WIFSTOPPED(status))
			{
				int rt = retcnt();

				fprintf(stderr,"[%d] Stopped ",rt+1);

				for(int ii=0;ii<k;ii++)
				{
					fprintf(stderr,"%s ",parsed[ii]);
				}
				printf("\n");
				signal(SIGCHLD, func);
				//	printf("pid is %d\n",forkreturn);
				//	setpgid(forkreturn , forkreturn);

				struct bgprocess *curr =  head;
				setpgid(forkreturn, forkreturn);
				if(curr == NULL)
				{
					struct bgprocess * child;
					child = (struct bgprocess *)malloc(sizeof(struct bgprocess));
					child->pid =  forkreturn;
					child->flag = 1;
					child->prev = NULL;
					child->next = NULL;
					strcpy(child->tokens, falsecommand);
					/*                      for(int jjj=0;jjj<k;jjj++)
								{
								child->dtokens[jjj]= (char * )malloc(1024*sizeof(char));
								child->dtokens[jjj]=parsed[jjj];
								}
								child->dtokens[k]= (char * )malloc(1024*sizeof(char));
								child->dtokens[k]=NULL;*/
					curr = child;
					head = curr;
				}
				else
				{

					struct bgprocess *child;
					child = (struct bgprocess *)malloc(sizeof(struct bgprocess));
					child->pid =  forkreturn;
					strcpy(child->tokens, falsecommand);
					child->next = NULL;
					child->flag = 0;
					while(curr->next)
					{
						curr = curr->next;
					}
					/*                      for(int jjj=0;jjj<k;jjj++)
								{
								child->dtokens[jjj]= (char * )malloc(1024*sizeof(char));
								child->dtokens[jjj]=parsed[jjj];
								}
								child->dtokens[k]= (char * )malloc(1024*sizeof(char));
								child->dtokens[k]=NULL;
								*/
					child->prev = curr;
					curr->next = child;

				}

				return -1;
			}
			else
			{
				perror(parsed[0]);

				return -1;
			}
		}
		return 0;
	}


	else
	{
		signal(SIGCHLD, func);
		int forkreturn = fork();
		if(forkreturn == -1)
		{
			fprintf(stderr,"UNABLE TO FORK!!!\n");
			error = 1;

			//	printf(":'(");
			return -1;
		}
		struct bgprocess *curr =  head;
		if(curr == NULL)
		{
			struct bgprocess * child;
			child = (struct bgprocess *)malloc(sizeof(struct bgprocess));
			child->pid =  forkreturn;
			child->flag = 0;
			child->prev = NULL;
			child->next = NULL;
			strcpy(child->tokens, falsecommand);
			/*			for(int jjj=0;jjj<k;jjj++)
						{
						child->dtokens[jjj]= (char * )malloc(1024*sizeof(char));
						child->dtokens[jjj]=parsed[jjj];
						}
						child->dtokens[k]= (char * )malloc(1024*sizeof(char));
						child->dtokens[k]=NULL;*/
			curr = child;
			head = curr;
		}
		else
		{

			struct bgprocess *child;
			child = (struct bgprocess *)malloc(sizeof(struct bgprocess));
			child->pid =  forkreturn;
			strcpy(child->tokens, falsecommand);
			child->next = NULL;
			child->flag = 0;
			while(curr->next)
			{
				curr = curr->next;
			}
			/*			for(int jjj=0;jjj<k;jjj++)
						{
						child->dtokens[jjj]= (char * )malloc(1024*sizeof(char));
						child->dtokens[jjj]=parsed[jjj];
						}
						child->dtokens[k]= (char * )malloc(1024*sizeof(char));
						child->dtokens[k]=NULL;
						*/
			child->prev = curr;
			curr->next = child;

		}
		if(forkreturn == 0)
		{
			//			printf("giff\n");
			setpgid(0,0);
			/*	for(int ii=0;ii<2000000000;ii++)
				{

				}*/
			//			fprintf(stderr,"first\n");
			signal(SIGTSTP,SIG_DFL);
			signal(SIGINT,SIG_DFL);
			signal(SIGCONT,SIG_DFL);

			//signal(SIGTTOU,SIG_IGN);

			/*	if(strcmp(parsed[0],"vim"))
				{for(int kk =0;kk<1000000000;kk++)
				{}}*/
			if (execvp(parsed[0], parsed) < 0)
			{
				error = 1;
				fprintf(stderr,"\n%s:Could not execute the command\n",parsed[0]);
				fflush(stdout);
				exit(errno);
			}
		}
		fprintf(stderr,"[+]\t%d\n",forkreturn);

		//	printf(":')");
		return 0;
	}
	//	                printf("return is %d\n", forkreturn);
}
/*void change()
  {
  printf("almost done \n");
  if(foregroundprocess>=0)
  {
  kill(foregroundprocess, SIGINT);
  }
  signal(SIGINT,:wq
  change);



  }*/
int main()
{
	signal(SIGTSTP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	getcwd(scwd, sizeof(scwd));
	scwd[strlen(scwd)]='\0';
	int exNum;
	char ccommandd[10024];
	strcpy(prevdir,scwd);
	int stdlen = strlen(scwd);
	//	printf("stdlen is %d\n",stdlen);
	printf(":')");
	printline(scwd);
	create_history(scwd);

	while(true)
	{
		int prev= 0;
		int F =0;
		struct bgprocess * curr = head;

		char bigcommand[10024];

		if(typedany(bigcommand))
		{
			//			printf("wrong");
			char command[10024];
			int kk =0;
			for(int i=0;i<strlen(bigcommand);i++)
			{
				if(bigcommand[i]==';')
				{
					command[i]='\0';
					strcpy(falsecommand, command);
					F = checkre2(command);
					prev = F;
					memset(command,'\0',sizeof(command));
					kk = i+1;

				}
				command[i-kk] = bigcommand[i];
			}
			strcpy(ccommandd, bigcommand);
			addhistory(ccommandd);
			strcpy(falsecommand, command);
			F = checkre2(command);
			if(F == 1)
			{
				if(prev== -1)
				{
					printf(":'(");
				}
				else
				{
					printf(":')");
				}
			}
			else
			{
				prev = F;
				if(prev== -1)
                                {
                                        printf(":'(");
                                }
                                else
                                {
                                        printf(":')");
                                }

			}

			printline(scwd);
			memset(command,'\0',sizeof(command));
			memset(bigcommand,'\0',sizeof(bigcommand));
			memset(ccommandd,'\0',sizeof(ccommandd));


		}
		else
		{
			continue;
		}
	}
	return 0;	
}

