#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_PROC 3

int main(void){
  pid_t pid;
	pid = fork();
  for (int i=0; i<NUM_PROC; i++){
  	if(pid <  0){
  		perror("fork");
  		exit(EXIT_FAILURE);
  	}
  	else if(pid > 0){
      fprintf(stdout, "j%d", (intmax_t)getpid())
      wait(NULL);
  		exit(EXIT_SUCCESS);
  	}
  }
  sleep(200);
	wait(NULL);
	exit(EXIT_SUCCESS);
}
