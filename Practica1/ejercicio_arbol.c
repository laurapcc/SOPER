/****************************************************************
* Nombre: ejercicio_arbol
* Autores: Jorge de Miguel y Laura de Paz
* Grupo de prácticas: 2202
* Fecha: 11/02/20
* Descripción: crea el esquema de la figura del pdf mediante
*              llamadas a la función fork()
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>


#define NUM_PROC 3

int main(void){
  pid_t pid;

  for (int i=1; i < NUM_PROC; i++){
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

  /* Hacemos esperar al proceso para comprobar el arbol de procesos en otra ventana de la terminal*/
  sleep(200);
	wait(NULL);
	exit(EXIT_SUCCESS);
}
