/****************************************************************
* Nombre: ejercicio_lect_escr
* Autores: Jorge de Miguel y Laura de Paz
* Grupo de prácticas: 2202
* Fecha: 21/03/2020
* Descripcion:
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>

#define N_READ 10
#define SECS 0
#define SEM_LECT "/sem_lect"
#define SEM_ESCR "/sem_escr"
#define SEM_AUX "/sem_aux"

int sigintRecibida=0;
int sigtermRecibida=0;

void lectura(){
  fprintf(stdout, "R-INI %ld\n", (long)getpid());
  fflush(stdout);
  sleep(1);
  fprintf(stdout, "R-FIN %ld\n", (long)getpid());
  fflush(stdout);
}

void escritura(){
  fprintf(stdout, "W-INI %ld\n", (long)getpid());
  fflush(stdout);
  sleep(1);
  fprintf(stdout, "W-FIN %ld\n", (long)getpid());
  fflush(stdout);
}

/* manejador: rutina de tratamiento de la señal SIGINT. */
void manejador(int sig) {
  sigintRecibida=1;
  return;
}

/* manejador: rutina de tratamiento de la señal SIGTERM. */
void manejador_sigterm(int sig) {
  sigtermRecibida=1;
  return;
}


int main(int argc, char* argv[]){
  sem_t *semLectura=NULL;
  sem_t *semEscritura= NULL;
  sem_t *semAux=NULL;
  pid_t array_pid[N_READ];
  pid_t pid;
  struct sigaction act;
  struct sigaction act2;

  if ((semLectura = sem_open(SEM_LECT, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	};
  if ((semEscritura = sem_open(SEM_ESCR, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	};
  if ((semAux = sem_open(SEM_AUX, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	};

  for (int i = 0; i < N_READ; i++){
    pid = fork();
    if (pid < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid){ /* padre */
        array_pid[i] = pid;
    }
    else{ /* hijo */
        break;
    }
  }

  sem_unlink(SEM_ESCR);
  sem_unlink(SEM_LECT);
  sem_unlink(SEM_AUX);

  if (pid){
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;

    /* Se arma la señal SIGINT. */
    act.sa_handler = manejador;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while (!sigintRecibida){
      sem_wait(semEscritura);
      escritura();
      sem_post(semEscritura);
      sleep(SECS);
    }

    /* enviar SIGTERM a todos los hijos */
    for (int i = 0; i < N_READ; i++){
        if (kill(array_pid[i], SIGTERM) < 0){
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }

    /* no dejar hijos huerfanos antes de finalizar */
    for (int i = 0; i < N_READ; i++) wait(NULL);

    sem_close(semEscritura);
    sem_close(semLectura);
    sem_close(semAux);
    exit(EXIT_SUCCESS);
  }
  else{
    int valSem;
    sigemptyset(&(act2.sa_mask));
    act2.sa_flags = 0;

    /* Se arma la señal SIGINT. */
    act2.sa_handler = manejador_sigterm;
    if (sigaction(SIGTERM, &act2, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while (!sigtermRecibida){
      sem_wait(semLectura);
      sem_post(semAux);
      sem_getvalue(semAux, &valSem);
      if (valSem==1)
        sem_wait(semEscritura);
      sem_post(semLectura);

      lectura();

      sem_wait(semLectura);
      sem_wait(semAux);
      sem_getvalue(semAux, &valSem);
      if (valSem==0)
        sem_post(semEscritura);
      sem_post(semLectura);
      sleep(SECS);
    }

    sem_close(semEscritura);
    sem_close(semLectura);
    sem_close(semAux);
    exit(EXIT_SUCCESS);
  }
}
