/****************************************************************
* Nombre: ejercicio_prottemp_mejorado
* Autores: Jorge de Miguel y Laura de Paz
* Grupo de prácticas: 2202
* Fecha: 17/03/2020
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

#define SEM1 "/sem1"
int usr2 = 0;
int N_global;
int fin_trabajo = 0;

/**
 * Nombre de la funcion: manejador
 * Parametros: sig: senal a manejar
 * Descripcion: funcion que se ejecutara cuando un proceso reciba la senal sig
 * Return:
 */
void manejador(int sig) {
    return;
}

/**
 * Nombre de la funcion: manejador_SIGUSR2
 * Parametros: sig: senal a manejar
 * Descripcion: funcion que se ejecutara cuando un proceso reciba la senal sig
 * Return:
 */
void manejador_SIGUSR2(int sig) {
    FILE *f = NULL;
    char buf[100];
    if ((f = fopen("data.txt", "r")) == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fgets(buf, 100, f);
    if (atoi(buf) == N_global){
        fin_trabajo = 1;
    }

    return;
}


/**
 * Nombre de la funcion: crear_txt
 * Parametros:
 * Descripcion: crea el archivo e inicializa las dos lineas a 0
 * Return:
 */
void crear_txt(){
    FILE *f = NULL;
    if ((f = fopen("data.txt", "w")) == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "0\n");
    fprintf(f, "0\n");
    fclose(f);
}

/**
 * Nombre de la funcion: trabajo_hijo
 * Parametros: suma
 * Descripcion: trabajo de lectura y escritura de un proceso hijo
 * Return:
 */
void trabajo_hijo(int suma){
    FILE *f = NULL;
    char buf[100];
    int proc_done;
    int sum_total;
    if ((f = fopen("data.txt", "r")) == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fgets(buf, 100, f);
    proc_done = atoi(buf);
    fgets(buf, 100, f);
    sum_total = atoi(buf);
    fclose(f);
    if ((f = fopen("data.txt", "w")) == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "%d\n", proc_done+1);
    fprintf(f, "%d\n", sum_total + suma);
    fclose(f);
}

/**
 * Nombre de la funcion: main
 * Parametros: argc: numero de parametros
 *             argv: array de strings con los paramtros
 * Descripcion: funcion principal del programa
 * Return: EXIT_FAILURE si algo falla durante la ejecucion
 * 		   EXIT_SUCCESS si se ejecuta correctamente
 */
int main(int argc, char* argv[]) {
    if (argc != 3){
        printf("El programa recibe exactamente 2 argumentos\n");
        printf("./ejercicio_prottemp.c <N> <T>\n");
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[1]);
    N_global = N;
    int T = atoi(argv[2]);
    pid_t array_pid[N];
    pid_t pid;
    sigset_t set;
    struct sigaction act;
    struct sigaction act_usr2;
    int i;

    sem_t *sem1 = NULL;
    if ((sem1 = sem_open(SEM1, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
  		perror("sem_open");
  		exit(EXIT_FAILURE);
	  }

    for (i = 0; i < N; i++){
        pid = fork();
        if (pid < 0){
            perror("fork");
            sem_unlink(SEM1);
            sem_close(sem1);
            exit(EXIT_FAILURE);
        }
        else if (pid){ /* padre */
            array_pid[i] = pid;
        }
        else{ /* hijo */
            break;
        }
    }

    sem_unlink(SEM1);

    if (pid){ /* padre */
        /* ignorar SIGALRM pero sin finalizar */
        sigemptyset(&(act.sa_mask));
        act.sa_flags = 0;
        act.sa_handler = manejador;
        if (sigaction(SIGALRM, &act, NULL) < 0) {
            perror("sigaction");
            sem_close(sem1);
            exit(EXIT_FAILURE);
        }

        /* ignorar SIGUSR2 pero sin finalizar */
        sigemptyset(&(act_usr2.sa_mask));
        act_usr2.sa_flags = 0;
        act_usr2.sa_handler = manejador_SIGUSR2;
        if (sigaction(SIGUSR2, &act_usr2, NULL) < 0) {
            perror("sigaction");
            sem_close(sem1);
            exit(EXIT_FAILURE);
        }

        /* creacion data.txt */
        crear_txt();
        sem_post(sem1);

        if (alarm(T)) {
            fprintf(stderr, "Existe una alarma previa establecida\n");
        }

        /* suspender todas senales hasta que reciba SIGALRM */
        sigfillset(&set);
        sigdelset(&set, SIGALRM);
        sigsuspend(&set);

        /* enviar SIGTERM a todos los hijos */
        if (fin_trabajo){
            for (i = 0; i < N; i++){
                if (kill(array_pid[i], SIGTERM) < 0){
                    perror("kill");
                    exit(EXIT_FAILURE);
                }
            }
        }

        /* no dejar hijos huerfanos antes de finalizar */
        for (i = 0; i < N; i++) wait(NULL);

        fprintf(stdout, "Finalizado padre\n");
        fprintf(stdout, "Se han recibido %d senales SIGUSR2\n", usr2);
        sem_close(sem1);
        exit(EXIT_SUCCESS);
    }

    // else
    /* ignorar SIGTERM pero sin finalizar */
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    act.sa_handler = manejador;
    if (sigaction(SIGTERM, &act, NULL) < 0){
        perror ("sigaction");
        sem_close(sem1);
        exit(EXIT_FAILURE);
    }

    int suma = 0;
    for (i = 1; i <= getpid()/10; i++) suma += i;
    fprintf(stdout, "PID = %jd\tSuma = %d\n",(intmax_t)getpid(), suma);

    /* espera a poder leer/escribir y entonces realiza tu trabajo */
    sem_wait(sem1);
    trabajo_hijo(suma); /* seccion critica */
    sem_post(sem1);

    /* el proceso ha finalizado su trabajo */
    if (kill(getppid(), SIGUSR2) < 0){
        perror("kill");
        sem_close(sem1);
        exit(EXIT_FAILURE);
    }

    /* suspender todas senales hasta que reciba SIGTERM */
    sigfillset(&set);
    sigdelset(&set, SIGTERM);
    sigsuspend(&set);

    /* ha recibido SIGTERM */
    fprintf(stdout, "Finalizado PID = %jd\n", (intmax_t)getpid());
    sem_close(sem1);
    exit(EXIT_SUCCESS);
}
