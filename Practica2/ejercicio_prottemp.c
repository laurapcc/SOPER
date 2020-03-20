/****************************************************************
* Nombre: ejercicio_prottemp
* Autores: Jorge de Miguel y Laura de Paz
* Grupo de prácticas: 2202
* Fecha: 13/03/2020
* Descripcion: 
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>


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
 * Nombre de la funcion: main
 * Parametros: argc: numero de parametros
 *             argv: array de strings con los paramtros
 * Descripcion: funcion principal del programa
 * Return: EXIT_FAILURE si algo falla durante la ejecucion
 * 		   EXIT_SUCCES si se ejecuta correctamente
 */
int main(int argc, char* argv[]) {
    if (argc != 3){
        printf("El programa recibe exactamente 2 argumentos\n");
        printf("./ejercicio_prottemp.c <N> <T>\n");
        exit(EXIT_FAILURE);
    }
    
    int N = atoi(argv[1]);
    int T = atoi(argv[2]);
    pid_t array_pid[N];
    pid_t pid;
    sigset_t set;
    struct sigaction act;
    int i;

    for (i = 0; i < N; i++){
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

    if (pid){ /* padre */

        sigemptyset(&(act.sa_mask));
        act.sa_flags = 0;
        act.sa_handler = manejador;

        /* ignorar SIGALRM pero sin finalizar */
        if (sigaction(SIGALRM, &act, NULL) < 0) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }

        /* ignorar SIGUSR2 pero sin finalizar */
        if (sigaction(SIGUSR2, &act, NULL) < 0) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }

        if (alarm(T)) {
            fprintf(stderr, "Existe una alarma previa establecida\n");
        }

        /* suspender todas senales hasta que reciba SIGALRM */
        sigfillset(&set);
        sigdelset(&set, SIGALRM);
        sigsuspend(&set);

        /* enviar SIGTERM a todos los hijos */
        for (i = 0; i < N; i++){
            if (kill(array_pid[i], SIGTERM) < 0){
                perror("kill");
                exit(EXIT_FAILURE);
            }
        }

        /* no dejar hijos huerfanos antes de finalizar */
        for (i = 0; i < N; i++) wait(NULL);

        fprintf(stdout, "Finalizado padre\n");
        exit(EXIT_SUCCESS);
    }
    
    // else
    int suma = 0;
    for (i = 1; i <= getpid()/10; i++) suma += i;
    fprintf(stdout, "PID = %jd\tSuma = %d\n",(intmax_t)getpid(), suma); 
    if (kill(getppid(), SIGUSR2) < 0){
        perror("kill");
        exit(EXIT_FAILURE);
    }
    
    /* ignorar SIGTERM pero sin finalizar */
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    act.sa_handler = manejador;
    if (sigaction(SIGTERM, &act, NULL) < 0){
        perror ("sigaction");
        exit(EXIT_FAILURE);
    }

    /* suspender todas senales hasta que reciba SIGTERM */
    sigfillset(&set);
    sigdelset(&set, SIGTERM);
    sigsuspend(&set);

    /* ha recibido SIGTERM */
    fprintf(stdout, "Finalizado PID = %jd\n", (intmax_t)getpid());

    exit(EXIT_SUCCESS);
}

