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

  /* Lectura de stdin hasta que reciba EOF */
  while((size=getline(&buffer, &num_bytes, stdin)) != EOF){
    buffer[size-1] = '\0'; /* Eliminacion de '\n' */
    pid = fork();

    if (pid < 0){
      perror("fork");
      exit(EXIT_FAILURE);
    }

    else if (pid > 0){ /* proceso padre */
      /* Espera a que el hijo termine su ejecucion y se muestra informacion sobre su estado */
      wait(&status); 
      if (WIFEXITED(status)){
        fprintf(stderr, "\nExited with value %d\n\n", WEXITSTATUS(status));
      }
      if (WIFSIGNALED(status)){
        fprintf(stderr, "\nTerminated by signal %d\n\n", WTERMSIG(status));
      }
    }

    else{ /* proceso hijo */
      /* Reconocimiento de lo leido en stdin como comando */
      if (wordexp(buffer, &command, 0) != 0){
        return EXIT_FAILURE;
      }

      /* Ejecucion del comando */
      execvp(command.we_wordv[0], command.we_wordv);
      wordfree(&command);
      exit(EXIT_SUCCESS);
    }
    fprintf(stdout, "\n");
  }

  free(buffer);
  return EXIT_SUCCESS;
}
