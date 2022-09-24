#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include<sys/wait.h>



#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

int bgProcesses[64];
int fg,bg; // Global variable for storing group id for foreground and background process - setpgid()

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

int killbg(int a[],int length)
{
  int i=0;
  while(i<length)
  {
    kill(a[i],SIGKILL);
    i++;
  }

}

void bghandler(int sig)
{
    printf("Inside Background process - Not terminating\n");
}

void fghandler(int sig)
{
    int pgid = getpgid(fg);
    printf("Killed foreground process id is %d\n",pgid);
    kill(fg,SIGKILL);
    
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i,status;
    int bg_process_id=0;

    // Handling CTRL + C 

    signal(SIGINT,bghandler); 
    signal(SIGINT,fghandler);
    
    while(1)
    {
        int bg_wait = waitpid(-1,NULL,WNOHANG);  //Reap any background proess before starting new command which has been completed so far
        
        if(bg_wait>0)
        
        printf("Shell: Background process finished\n");
			
	    /* BEGIN: TAKING INPUT */

		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
    
        int k;
        int length=0;
        for ( k = 0; tokens[k]!=NULL;k++)
        {
            length++;
        }
    
        if(tokens[0] == NULL)
		{
			continue;
		}
        printf("COMMAND = %s\n",tokens[0]);

        if(strcmp(tokens[0],"exit") == 0)
        {
            int i=0;
            int n=sizeof(bgProcesses)/sizeof(bgProcesses[0]);
            killbg(bgProcesses,n);
            int x = kill(getpid(), SIGKILL);
        
        }
        /* IMPLEMENTING "cd" via CHDIR() -> Prints the path of the directory , shows current working directory 
        and after chdir() displays new working directory */

        if(strcmp(tokens[0],"cd") == 0)
		{
        
            printf("PATH for cd = %s\n",tokens[1]);
            char s[100];
    	    printf("Now current is :  %s\n",getcwd(s,100));
            if(tokens[2])
            {
                printf("Invalid CD command\n");
                continue;
            }
    	
    	    int chdir_exit_status = chdir(tokens[1]);
        
            if(chdir_exit_status==0)
            {
                printf("Directory succesfully changed to = %s\n",getcwd(s,100));
            }
            else{
                printf("CD Command failed\n");
            }
            continue;
        }

		int fc=fork();
		if(fc<0)
		{ 
			fprintf(stderr,"%s\n","Unable to create child process !!\n");

		}
		else if(fc==0) //Child process
		{
            if(strcmp(tokens[length-1],"&") == 0)
            {
                tokens[length-1]=NULL;
                execvp(tokens[0],tokens);
            }

            else
            {
             
			  execvp(tokens[0],tokens);
              printf("Command execution failed\n");
			  _exit(1);
		    }
        }
        else
		{
            if(strcmp(tokens[length-1],"&") == 0) // User has entered & to run it in background
            {
                bgProcesses[bg_process_id++]=fc;  // Adding background process ids to an array to kill it one by one on loop on "exit"
                bg=fc;
                setpgid(fc,bg);
                int wait_pid_result = waitpid(fc,&status,WNOHANG);
            }
      
            else
            {
                fg=fc;
                setpgid(fc,fg);
			    int wc = wait(NULL); //parent process wait till child process completes.
            }
        }   
       
         // Freeing the allocated memory	
		    for(i=0;tokens[i]!=NULL;i++){
			    free(tokens[i]);
		    }
		    free(tokens);

	}
	return 0;
}    

    
	