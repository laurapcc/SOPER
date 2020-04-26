#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>
#include <mqueue.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include "sort.h"

#define SHM_NAME "/shm_proyecto"
#define MQ_NAME "/mq_proyecto"

/* Pipes */
#define READ_PIPE 0
#define WRITE_PIPE 1


/* Manejador de la senhal SIGUSR1 */
void manejador(int sig){
    return;
}

/* Manejador de la senhal SIGINT */
void manejador_SIGINT(int sig){
    return;
}


int main(int argc, char *argv[]) {
    int i, retval;
    int n_levels, n_processes, delay;
    char* file;
    
    struct sigaction actUsr1;
    struct sigaction actInt;
    sigset_t setUsr1, set2;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <FILE> <N_LEVELS> <N_PROCESSES> [<DELAY>]\n", argv[0]);
        fprintf(stderr, "    <FILE> :        Data file\n");
        fprintf(stderr, "    <N_LEVELS> :    Number of levels (1 - %d)\n", MAX_LEVELS);
        fprintf(stderr, "    <N_PROCESSES> : Number of processes (1 - %d)\n", MAX_PARTS);
        fprintf(stderr, "    [<DELAY>] :     Delay (ms)\n");
        exit(EXIT_FAILURE);
    }

    file = argv[1];
    n_levels = atoi(argv[2]);
    n_processes = atoi(argv[3]);
    if (argc > 4) {
        delay = 1e6 * atoi(argv[4]);
    }
    else {
        delay = 1e8;
    }

    /***************** MEMORIA COMPARTIDA *****************/
    /* Abrimos el segmento de memoria compartida */
    int fd_shm = shm_open(SHM_NAME,
        O_RDWR | O_CREAT | O_EXCL,
        S_IRUSR | S_IWUSR);
    
    if (fd_shm == -1) {
        fprintf(stderr, "Error abriendo el segmento de memoria\n");
        return EXIT_FAILURE;
    }

    /* Truncamos el segmento de memoria compartida */
    if (ftruncate(fd_shm, sizeof(Sort)) == -1) { 
        fprintf(stderr, "Error al incrementar el tamanho del segmento de memoria\n");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /* Mapeamos el segmento de memoria compartida */
    Sort *s = mmap(NULL, sizeof(*s),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd_shm,
        0);
    close(fd_shm);

    if (s == MAP_FAILED) {
        fprintf(stderr, "Error mapeando el segmento de memoria\n");
        return EXIT_FAILURE;
    }

    /********** INICIALIZACION DE LA ESTRUCTURA ***********/
    if (init_sort(file, s, n_levels, n_processes, delay) == ERROR){
        fprintf(stderr, "Error al crear la estructura sort\n");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /****************** COLA DE MENSAJES ******************/
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = sizeof(Task)
    };

    mqd_t queue = mq_open(MQ_NAME,
        O_CREAT | O_WRONLY, 
        S_IRUSR | S_IWUSR, 
        &attributes);

    if (queue == (mqd_t)-1) {
        fprintf(stderr, "Error opening the queue\n");
        return EXIT_FAILURE;
    }

    /********** TUBERIAS TRABAJADOR - ILUSTRADOR **********/
    int pipe_trab2ilust[n_processes][2]; /* n_processes trabajadores y 1 ilustrador */
    int pipe_ilust2trab[n_processes][2];
    for (i = 0; i < n_processes; i++){
        if (pipe(pipe_trab2ilust[i]) == -1 || pipe(pipe_ilust2trab[i]) == -1){
            fprintf(stderr, "Error al crear las tuberia %d\n",i);
            shm_unlink(SHM_NAME);
            mq_unlink(MQ_NAME);
            return EXIT_FAILURE;
        }
    }

    /********************* SEMAFOROS **********************/
    //! cuantos semaforos hay que crear?? uno por tarea??


    /*************** OTRAS CONFIGURACIONES ****************/
    /* Manejamos SIGUSR1 */
    sigemptyset(&(actUsr1.sa_mask));
    actUsr1.sa_flags = 0;
    actUsr1.sa_handler = manejador;
    if (sigaction(SIGUSR1, &actUsr1, NULL) < 0) {
    	fprintf(stderr, "Error in sigaction (SIGUSR1)\n");
		shm_unlink(SHM_NAME);
        mq_unlink(MQ_NAME);
        // TODO : liberar semaforos
    	return EXIT_FAILURE;
    }

    /* Bloqueamos la señal SIGUSR1 para evitar que llegue antes de tiempo */
    sigemptyset(&setUsr1);
    sigaddset(&setUsr1, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &setUsr1, NULL) < 0) {
        fprintf(stderr, "Error in sigprocmask\n");
		shm_unlink(SHM_NAME);
        mq_unlink(MQ_NAME);
        // TODO : liberar semaforos
        return EXIT_FAILURE;
    }

    /* Manejamos SIGINT */
    // sigemptyset(&(actInt.sa_mask));
    // actInt.sa_flags = 0;
    // actInt.sa_handler = manejador_SIGINT;
    // if (sigaction(SIGINT, &actInt, NULL) < 0) {
    // 	fprintf(stderr, "Error in sigaction (SIGINT)\n");
	// 	shm_unlink(SHM_NAME);
    //     mq_unlink(MQ_NAME);
    //     // TODO : liberar semaforos
    // 	return EXIT_FAILURE;
    // }

    /***************** CREACION PROCESOS ******************/
    

    //return sort_single_process(argv[1], n_levels, n_processes, delay);
    shm_unlink(SHM_NAME);
    retval = sort_multiple_processes(s);
    munmap(s, sizeof(*s));
    mq_unlink(MQ_NAME);
    return retval;
}
