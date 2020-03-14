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
 * Nombre de la funcion: manejador_SIGTERM
 * Parametros: sig: senal a manejar
 * Descripcion: funcion que se ejecutara cuando un proceso reciba la senal sig
 * Return: 
 */
void manejador_SIGTERM(int sig) {
    fprintf(stdout, "Finalizado PID = %jd\n", (intmax_t)getpid());
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
    
    pid_t *pid = NULL;
    int N = atoi(argv[1]);
    int T = atoi(argv[2]);
    int i,j;

    pid = (pid_t *)malloc(N*sizeof(pid_t));
    if (pid == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* primer hijo */
    pid[0] = fork();
    if (pid[0] < 0){
        perror("fork");
        free(pid);
        exit(EXIT_FAILURE);
    }
    
    /* resto de hijos */
    for (i = 1; i < N; i++){
        if (pid[i-1] > 0){ /* padre */ 
            pid[i] = fork();
            if (pid[i] < 0){
                perror("fork");

                for (j = 0; j < i; j++){
                    /* matar a los hijos */
                }

                free(pid);
                exit(EXIT_FAILURE);
            }       
        }
    }
    
    int padre = 1;
    for (i = 0; i < N; i++){
        if (pid[i] == 0){
            padre = 0;
            break;
        }
    }
    if (padre){
        if (alarm(T)) {
            fprintf(stderr, "Existe una alarma previa establecida\n");
        }

        sigset_t set;
        sigfillset(&set);
        sigdelset(&set, SIGALRM);
        sigsuspend(&set);

        for (i = 0; i < N; i++){
            printf("aa = %d\n",i);
            kill(pid[i], SIGTERM);
            wait(NULL); // ???
        }
        free(pid);
        fprintf(stdout, "Finalizado padre");
        exit(EXIT_SUCCESS);
    }

    long int suma = 0;
    for (i = 1; i <= (int)(getpid()/10); i++) suma += i;
    fprintf(stdout, "PID = %jd\tSuma = %ld\n",(intmax_t)getpid(), suma); 
    
    kill(getppid(), SIGUSR2);

    struct sigaction act_hijo;
    sigfillset(&(act_hijo.sa_mask));
    sigdelset(&(act_hijo.sa_mask), SIGTERM);
    act_hijo.sa_flags = 0;
    act_hijo.sa_handler = manejador_SIGTERM;

    if (sigaction(SIGTERM, &act_hijo, NULL) < 0){
        perror ("sigaction");
        free(pid);
        exit(EXIT_FAILURE);
    }
    
    free(pid);
    exit(EXIT_SUCCESS);
}
