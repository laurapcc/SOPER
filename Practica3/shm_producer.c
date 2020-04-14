#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "cola.h"

#define SHM_NAME "/shm_consprod"

typedef struct {
	sem_t empty;    /* Anonym semaphore */
    sem_t full;     /* Anonym semaphore */
	sem_t mutex;    /* Anonym semaphore */
    Cola cola;
} Estructura;

int main(int argc, char *argv[]) {
    int n, modo;

    srand (time(NULL));

    if (argc!=3){
        fprintf(stdout, "Error en los argumentos de entrada");
        return EXIT_FAILURE;
    }

    /* argumentos de entrada */
    n = atoi(argv[1]);
    modo = atoi(argv[2]);

    /* Abrimos el segmento de memoria compartida */
    int fd_shm = shm_open(SHM_NAME,
        O_RDWR | O_CREAT | O_EXCL,
        S_IRUSR | S_IWUSR);

    if (fd_shm == -1) {
        fprintf(stderr, "Error creating the shared memory segment\n");
        return EXIT_FAILURE;
    }

    /* Truncamos el segmento de memoria compartida */
    if (ftruncate(fd_shm, sizeof(Estructura)) == -1) {
        fprintf(stderr, "Error resizing the shared memory segment\n");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /* Mapeamos el segmento de memoria */
    Estructura *e = mmap(NULL,
        sizeof(*e),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd_shm,
        0);
    close(fd_shm);

    if (e == MAP_FAILED) {
        fprintf(stderr, "Error mapping the shared memory segment\n");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }

    /* inicializamos la cola de la estructura en memoria compartida */
    e->cola = *cola_ini();
    printf("Pointer to shared memory segment: %p\n", (void*)e);

    /* inicializamos los semaforos */
    if (sem_init(&(e->empty), 1, MAX_COLA) == -1){
		fprintf(stderr, "Error creating semaphore\n");
		shm_unlink(SHM_NAME);
		return EXIT_FAILURE;
	}
    if (sem_init(&(e->full), 1, 0) == -1){
		fprintf(stderr, "Error creating semaphore\n");
		shm_unlink(SHM_NAME);
		return EXIT_FAILURE;
	}
    if (sem_init(&(e->mutex), 1, 1) == -1){
		fprintf(stderr, "Error creating semaphore\n");
		shm_unlink(SHM_NAME);
		return EXIT_FAILURE;
	}

    switch (modo){
    case 0: // aleatorio
        for (int i=0; i<n; i++){
            sem_wait(&(e->empty));
            sem_wait(&(e->mutex));
            insertar(&(e->cola), rand()%10);
            sem_post(&(e->mutex));
            sem_post(&(e->full));
            sleep(1);
        }
        break;
    case 1: // secuencial
        for (int i=0; i<n; i++){
            sem_wait(&(e->empty));
            sem_wait(&(e->mutex));
            insertar(&(e->cola), i%10);
            sem_post(&(e->mutex));
            sem_post(&(e->full));
            sleep(1);
        }
        break;
    default:
        break;
    }

    /* incluir -1 en la cola al terminar */
    sem_wait(&(e->empty));
    sem_wait(&(e->mutex));
    insertar(&(e->cola), -1);
    sem_post(&(e->mutex));
    sem_post(&(e->full));
    sleep(1);

    /* The daemon executes until press some character */
    printf("Introduce un caracter para terminar");
    getchar();

    /* Free the structure and shared memory */
    sem_destroy(&(e->mutex));
    sem_destroy(&(e->empty));
    sem_destroy(&(e->full));
    munmap(e, sizeof(*e));
    shm_unlink(SHM_NAME);

    return EXIT_SUCCESS;
}