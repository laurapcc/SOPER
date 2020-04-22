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


/* Manejador de la senhal SIGUSR1 */
void manejador(int sig){
    return;
}


int main(int argc, char *argv[]) {
    int n_levels, n_processes, delay;
    char* file;
    /*struct sigaction act;
    sigset_t setUsr1, set2;*/

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

    if (init_sort(file, s, n_levels, n_processes, delay) == ERROR){
        fprintf(stderr, "Error al crear la estructura sort\n");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    //!
    //TODO: Inicializar los semaforos, las tuberias y la cola de mensajes
    //!

    /* Bloqueamos la señal SIGUSR1 para evitar que llegue antes de tiempo */
    /*sigemptyset(&setUsr1);
    sigaddset(&setUsr1, SIGUSR1);
    sigprocmask(SIG_BLOCK, &setUsr1, NULL);*/

    /* manejar SIGUSR1 */
    /*sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    act.sa_handler = manejador;
    if (sigaction(SIGUSR1, &act, NULL) < 0) {
    	fprintf(stderr, "Error in sigaction\n");
		shm_unlink(SHM_NAME);
    	return EXIT_FAILURE;
    }*/




    return sort_single_process(/*argv[1], n_levels, n_processes, delay*/s);
}
