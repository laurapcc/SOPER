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
  char *buffer=NULL;
  size_t num_bytes;
  wordexp_t command;
  int status;

  while(getline(&buffer, &num_bytes, stdin) != EOF){
    buffer[strlen(buffer)-1] = '\0';
    pid = fork();

    if (pid < 0) return EXIT_FAILURE;

    else if (pid > 0){ /* padre */
      wait(&status);
      if (WIFEXITED(status)){
        fprintf(stderr, "Exited with value %d\n", WEXITSTATUS(status));
      }
      fprintf(stderr, "Terminated by signal %d\n", WTERMSIG(status));

    }

    else{ /* hijo */
      if (wordexp(buffer, &command, 0) != 0){
        return EXIT_FAILURE;
      }
      execvp(command.we_wordv[0], command.we_wordv);
      wordfree(&command);
    }
    fprintf(stdout, "\n");
  }

  return EXIT_SUCCESS;
}
