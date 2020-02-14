#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wordexp.h>


int main(){
  pid_t pid;
  char *buffer;
  size_t num_bytes;
  wordexp_t command;
  int status;


  
  while(getline(&buffer, &num_bytes, stdin) != EOF){
    pid = fork();
    if (pid < 0) return EXIT_FAILURE;
    else if (pid > 0){ /* padre */
      wait(&status);
      perror("Exited with value");
    }
    else{ /* hijo */
      if (wordexp(buffer, &command, 0) != 0){
        wordfree(&command);
        return EXIT_FAILURE;
      }
      execv(command.we_wordv[0], command.we_wordv);
      wordfree(&command);
    }
  }
  

  return EXIT_SUCCESS;
}