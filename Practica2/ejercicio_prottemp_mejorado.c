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
#define SEM2 "/sem2"
int usr2 = 0;
int N_global;
int fin_trabajo = 0;

/**
 * Nombre de la funcion: manejador_SIGTERM
 * Parametros: sig: senal a manejar
 * Descripcion: ignora la funcion SIGTERM pero no termina el programa
 * Return:
 */
void manejador_SIGTERM(int sig);


/**
 * Nombre de la funcion: manejador_SIGUSR2
 * Parametros: sig: senal a manejar
 * Descripcion: cuando recibe la senal SIGUSR2 comprueba si todos los procesos hijos
 *              han terminado ya su trabajo
 * Return:
 */
void manejador_SIGUSR2(int sig);


/**
 * Nombre de la funcion: manejador_SIGALRM
 * Parametros: sig: senal a manejar
 * Descripcion: si recibe la senal SIGALARM antes de que los hijos hayan terminado
 *              su trabajo, entonces se ejecurara esta funcion
 * Return:
 */
void manejador_SIGALRM(int sig);


/**
 * Nombre de la funcion: crear_txt
 * Parametros:
 * Descripcion: crea el archivo data.txt e inicializa las dos lineas a 0
 * Return:
 */
void crear_txt();


/**
 * Nombre de la funcion: trabajo_hijo
 * Parametros: suma
 * Descripcion: trabajo de lectura y escritura en data.txt de un proceso hijo
 * Return:
 */
void trabajo_hijo(int suma);


/**
 * Nombre de la funcion: fin_antes_tiempo
 * Parametros: suma
 * Descripcion: funcion que indica que han acabado todos los procesos hijos antes
 *              de que el padre haya recibido la senal SIGALRM
 * Return:
 */
void fin_antes_tiempo();


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
        act.sa_handler = manejador_SIGALRM;
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
        sigdelset(&set, SIGUSR2);
        sigsuspend(&set);

        /* enviar SIGTERM a todos los hijos */
        // PROBLEMA: llega aqui antes de tiempo y por eso no da tiempo a ejecutar todos los hijos
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

        fin_antes_tiempo();
        sem_close(sem1);
        exit(EXIT_SUCCESS);
    }

    // else
    /* ignorar SIGTERM pero sin finalizar */
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    act.sa_handler = manejador_SIGTERM;
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


/*****************************************************/
/*************** FUNCIONES MANEJADORAS ***************/
/*****************************************************/

void manejador_SIGTERM(int sig) {
    return;
}


void manejador_SIGUSR2(int sig) {
    FILE *f = NULL;
    char buf[100];
    usr2++;
    if ((f = fopen("data.txt", "r")) == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fgets(buf, 100, f);
    if (atoi(buf) == N_global){
        fin_trabajo = 1;
    }

    printf("fin_trabajo = %d\n", fin_trabajo);

    return;
}


void manejador_SIGALRM(int sig) {
    fprintf(stdout, "Falta trabajo\n");
    return;
}


/*****************************************************/
/****************** OTRAS FUNCIONES ******************/
/*****************************************************/

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


void fin_antes_tiempo(){
    FILE *f = NULL;
    char buf[100];
    if ((f = fopen("data.txt", "r")) == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fgets(buf, 100, f);
    fgets(buf, 100, f);
    fprintf(stdout, "Han acabado todos, resultado: %s\n", buf);
}