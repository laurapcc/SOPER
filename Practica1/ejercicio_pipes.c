/****************************************************************
* Nombre: ejercicio_pipes
* Autores: Jorge de Miguel y Laura de Paz
* Grupo de prácticas: 2202
* Fecha: 19/02/20
* Descripción: crea 2 procesos hijos y 2 pipes mediante las
* 						 cuales se comunica con ellos
****************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define FILE1 "numero_leido.txt"

int main(void) {
	/* creacion de la primera pipe */
	int fd1[2]; 
	int pipe_status1 = pipe(fd1);
	if(pipe_status1 == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid_t childpid = fork();
	if(childpid == -1){ 
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (childpid == 0) { /* proceso hijo 1 */
		/* Cierre del descriptor de entrada en el hijo 1 */
		close(fd1[0]);
		srand(time(NULL));

		/* Enviar el número aleatorio vía descriptor de salida */
		/* sizeof(int) < PIPE_BUF así que no hay escrituras cortas */
    int aleat = rand()%100 + 1; /* numero aleatorio entre 1 y 100 */
    ssize_t nbytes = write(fd1[1], &aleat, sizeof(int));
		if(nbytes == -1){
			perror("write");
			exit(EXIT_FAILURE);
		}

		printf("He escrito el número %d en el pipe\n", aleat);
		exit(EXIT_SUCCESS);
	}

  else { /* proceso padre */
		/* Cierre del descriptor de salida en el padre */
		close(fd1[1]);

		/* Creación de la segunda pipe */
		int fd2[2];
		int pipe_status2 = pipe(fd2);
		if(pipe_status2 == -1){
			perror("pipe");
			exit(EXIT_FAILURE);
		}

    /* Creación del segundo hijo */
    pid_t childpid2 = fork();
  	if(childpid2 == -1){
  		perror("fork");
  		exit(EXIT_FAILURE);
  	}

    if (childpid2 == 0) { /* proceso hijo 2 */
      /* Cierre del descriptor de salida en el segundo hijo */
  		close(fd2[1]);

			/* Lectura del numero aleatorio desde el descriptor de entrada */
      ssize_t nbytes2 = 0;
      int readNum2;
      nbytes2 = read(fd2[0], &readNum2, sizeof(int));
      if(nbytes2 == -1){
        perror("read");
        exit(EXIT_FAILURE);
      }

      if(nbytes2 > 0){
				/* Apertura del fichero en el que se escribira el numero aleatorio leido */
        int file = open(FILE1, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if(file == -1){
      		perror("open");
      		exit(EXIT_FAILURE);
      	}
				/* Escritura en el fichero mediante el su descriptor */
        ssize_t size_written = dprintf(file, "%d\n", readNum2);
    		if(size_written == -1){
          close(file);
    			perror("write");
    			exit(EXIT_FAILURE);
    		}
        close(file);
        exit(EXIT_SUCCESS);
      }
    }

    else{ /* Proceso padre */
      /* Cierre del descriptor de entrada en el padre */
  		close(fd2[0]);

  		ssize_t nbytes3 = 0;
			int readNum;

			/* Lectura del numero aleatorio desde el descriptor de entrada */
			nbytes3 = read(fd1[0], &readNum, sizeof(int));
			if(nbytes3 == -1){
				perror("read");
				exit(EXIT_FAILURE);
			}
			if(nbytes3 > 0){
				/* Enviar el número aleatorio vía descriptor de salida */
        ssize_t nbytesEnv = write(fd2[1], &readNum, sizeof(int));
    		if(nbytesEnv == -1){
    			perror("write");
    			exit(EXIT_FAILURE);
    		}
  		}
      wait(NULL);
    }

		wait(NULL);
		exit(EXIT_SUCCESS);
	}
}
