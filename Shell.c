/**********************************************************************
* Shell
* version: 3.0
* sep, 16th, 2015
*
* This lib was written by thangdn
* Contact:thangdn.tlu@outlook.com
*
* Every comment would be appreciated.
*
* If you want to use parts of any code of mine:
* let me know and
* use it!
**********************************************************************/
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <errno.h>  
#include <signal.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFSZ 100
#define BUFFER_SIZE 80 
#define ARR_SIZE 80    
#define HISTORY_SIZE 10
#define HISTORY_SIZE20 20
static     char buffer[BUFFER_SIZE]; 
char *history[HISTORY_SIZE];      
int history_count = 0 ;       
int count=0;
char *history20[HISTORY_SIZE20];      
int history_count20 = 0 ;       
int count20=0;

void add_command(char *command) 
{	
	count++;
	int i; 
	if( history_count < HISTORY_SIZE )
	{
		history[history_count++] = strdup(command); 
	}
	else
	{
		free(history[0]);   
		for(i=1;i<HISTORY_SIZE;i++) 
		{
			history[i-1] = history[i];
		}
		history[HISTORY_SIZE -1] = strdup(command); 
	}
}

void list_history()
{
	int i;	
	
	for(i=0;i<HISTORY_SIZE;i++)
		{
			if(history[i]==NULL)
			{
				break;
			}
			printf("[%d] %s",i,history[i]);
		}
}
///
void add_command20(char *command) 
{	
	count20++;
	int i; 
	if( history_count20 < HISTORY_SIZE20 )
	{
		history20[history_count20++] = strdup(command); 
	}
	else
	{
		free(history20[0]);   
		for(i=1;i<HISTORY_SIZE20;i++) 
		{
			history20[i-1] = history20[i];
		}
		history20[HISTORY_SIZE20 -1] = strdup(command); 
	}
}

void list_history20()
{
	int i;	
	
	for(i=0;i<HISTORY_SIZE20;i++)
		{
			if(history20[i]==NULL)
			{
				break;
			}
			printf("[%d] %s",i,history20[i]);
		}
}

int setup (char buffer[] , char *args[], int *background ) //
{
    
    char *pch;
    int i=0,j=0,h=0; 
 
    pch = strtok (buffer," \t\n");
    while (pch != NULL)
    {
	if(*pch == '&') 	
	{
		args[i] = '\0';
		*background = 1;
		break;
	}
	args[i] = pch;
	i++;
        pch = strtok (NULL," \t\n");	
    }
	args[i] = NULL;
    return 0;
}

void handle_SIGQUIT()
  {

	printf("\nTong so luong lenh:%d ", count);
	printf("\n 10 Lenh gan nhat : \n");
	list_history();
	printf("\nTong so luong lenh dung nhat:%d ", count20);
	printf("\n 10 Lenh dung gan nhat : \n");
	list_history20();
  }


void signal_Handler()
{
	struct sigaction handler;
	handler.sa_handler = handle_SIGQUIT;
	sigemptyset(&handler.sa_mask);
	handler.sa_flags = 0;
	sigaction(SIGQUIT, &handler, NULL);
	strcpy(buffer,"Caught C0ntrol C \n");	
}
int main()
{
    char *args[ARR_SIZE];
    int background;
    int i=0;	
    pid_t pid;  
    signal_Handler(); 
  
    int segment_id;
	char *checkpp;
	const int size1=5;
	segment_id = shmget(IPC_PRIVATE,size1,S_IRUSR|S_IWUSR);
	checkpp=(char*)shmat(segment_id,NULL,0);
	sprintf(checkpp,"false");
   

    int file_pipes[2];
	int fdin;
	char buf[BUFSZ];
	if (pipe(file_pipes)<0)
		perror("pipe");

    while(1) 
    {
		background = 0 ;  
		printf("COMMAND-> ");
		fflush (stdout); 
		if(fgets(buffer, sizeof(buffer), stdin)!=NULL) 
		{
			fflush(stdin);
			add_command(buffer); 
			char buffer20[BUFFER_SIZE];
			sprintf(buffer20,"%s",buffer);
			setup(buffer, args,&background) ; 
			

			if (!strcmp(args[0], "exit" )) 
				exit(1);     
			pid = fork(); 

			if(pid<0)
			{
				fprintf(stderr,"Fork Failed"); 
				return 1;
			}
			else if(pid == 0)
			{
				dup2(file_pipes[1],1);
				if(!strcmp(args[0],"pp"))
				{
					sprintf(checkpp,"true");
				}
				if(!strcmp(args[0],"po"))
				{
					sprintf(checkpp,"false");
				}

				if (!strcmp(args[0], "ll" ))
				{
					char *argm[] = {"ls", "-l"}; 
      				execvp(argm[0], argm);
				} else
				execvp(args[0],args);

				exit(0);
			}
			else{
				if(!strcmp(checkpp,"true"))
				{
					printf("In ra file\n" );
					int le=open("output.txt", O_CREAT|O_RDWR, 0666);
					close(file_pipes[1]);
					int len;
					int i=0;
					while((len = read(file_pipes[0],buf,BUFSZ))>0)
					{	
						write(le,buf,len);
						i++;
					}

					if(i!=0)
					{
						add_command20(buffer20); 
					}
					close(file_pipes[0]);
					close(le);
				}
				if(!strcmp(checkpp,"false"))
				{
					printf("In ra man hinh\n");
					close(file_pipes[1]);
					int len;
					int i=0;
					while((len = read(file_pipes[0],buf,BUFSZ))>0)
					{	
						write(1,buf,len);
						i++;
					}
					if(i!=0)
					{
						add_command20(buffer20); 
					}
					close(file_pipes[0]);
				}

				if(background == 0)
				{	
					waitpid(pid,NULL,0); 
				}
			}
    	}    
	}
	return 0;

}


