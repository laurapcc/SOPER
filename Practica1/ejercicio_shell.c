/****************************************************************
* Nombre: ejercicio_shell
* Autores: Jorge de Miguel y Laura de Paz
* Grupo de prácticas: 2202
* Fecha: 15/02/2020
* Descripción: crea una shell en la que ejecutar comandos
*              mediante el uso de fork() y execv() 
****************************************************************/

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
  int size;
  int status;

  while((size=getline(&buffer, &num_bytes, stdin)) != EOF){
    buffer[size-1] = '\0';
    pid = fork();

    if (pid < 0) return EXIT_FAILURE;

    else if (pid > 0){ /* padre */
      wait(&status);
      if (WIFEXITED(status)){
        fprintf(stderr, "\nExited with value %d\n\n", WEXITSTATUS(status));
      }
      if (WIFSIGNALED(status)){
        fprintf(stderr, "\nTerminated by signal %d\n\n", WTERMSIG(status));
      }
    }

    else{ /* hijo */
      if (wordexp(buffer, &command, 0) != 0){
        return EXIT_FAILURE;
      }
      execvp(command.we_wordv[0], command.we_wordv);
      wordfree(&command);
      exit(EXIT_FAILURE);
    }
    fprintf(stdout, "\n");
  }

  free(buffer);
  return EXIT_SUCCESS;
}
