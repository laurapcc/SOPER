#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>


#define NUM_PROC 3

int main(void){
  pid_t pid;
	
  for (int i=0; i < NUM_PROC; i++){
		pid = fork();
  	if(pid <  0){
  		perror("fork");
  		exit(EXIT_FAILURE);
  	}
  	else if(pid > 0){
      fprintf(stdout, "PID = %jd\n", (intmax_t)getpid());
			wait(NULL);
   		exit(EXIT_SUCCESS);
  	}
		else if (pid == 0){
			fprintf(stdout, "PID = %jd   PPID = %jd\n",(intmax_t)getpid(), (intmax_t)getppid());
		}
  }
	wait(NULL);
	exit(EXIT_SUCCESS);
}
