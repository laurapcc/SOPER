#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define READ 0
#define WRITE 1

int main(void) {
	int fd1[2];
  int fd2[2];

	char readbuffer[80];
  
	int pipe_status1 = pipe(fd1);
  int pipe_status2 = pipe(fd2);
 	if(pipe_status1 == -1 || pipe_status2 == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid_t pid1 = fork();
	if(pid1 == -1){
		perror("fork1");
		exit(EXIT_FAILURE);
	}

	if (pid1 == 0) { /* hijo 1 */
		close(fd1[READ]);

    srand(time(NULL));
    int x = rand()%100 + 1; /* num aleatorio entre 1 y 100 */
    char num[20];
    sprintf(num, "%d", x);
    
		ssize_t nbytes = write(fd1[WRITE], num, sizeof(num)+1);
		if(nbytes == -1){
			perror("write hijo");
			exit(EXIT_FAILURE);
		}

		printf("x = %d\n", x);
		exit(EXIT_SUCCESS);
	}

  else { /* padre */
    close(fd1[WRITE]);
    pid_t pid2 = fork();

    if (pid2 == -1){
      perror("fork2");
		  exit(EXIT_FAILURE);
    }

    if (pid2 == 0){ /* hijo 2 */
      close(fd2[WRITE]);
      ssize_t nbytes = 0;
      readbuffer[0] = '\0';
      do {
        nbytes = read(fd2[READ], readbuffer, sizeof(readbuffer));
        if(nbytes == -1){
          perror("read");
          exit(EXIT_FAILURE);
        }
      } while(nbytes != 0);

      printf("hijo2 escribe: x = %s\n", readbuffer);

      FILE *f = NULL;
      f = fopen("numero_leido.txt", "w");
      if (f == NULL) {
        perror("file");
        exit(EXIT_FAILURE);
      }
      if (fprintf(f, "%s", readbuffer) < 0){
        perror("fprintf");
        exit(EXIT_FAILURE);
      }
      
      fclose(f);
      exit(EXIT_SUCCESS);
    }

    else{ /* padre */
      close(fd2[READ]);

      /* leer */
      ssize_t nbytes = 0;
      do {
        nbytes = read(fd1[READ], readbuffer, sizeof(readbuffer));
        if(nbytes == -1){
          perror("read");
          exit(EXIT_FAILURE);
        }
      } while(nbytes != 0);

      /* escribir */
      nbytes = write(fd2[WRITE], readbuffer, strlen(readbuffer) + 1);
      if(nbytes == -1){
        perror("write padre");
        exit(EXIT_FAILURE);
      }
      
      wait(NULL);
      exit(EXIT_SUCCESS);
    }
		
		
	}
}

