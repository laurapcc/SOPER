#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cola.h"

#define SHM_NAME "/shm_consprod"
#define MAX_ARRAY 100

typedef struct {
	sem_t empty;    /* Anonym semaphore */
    sem_t full;     /* Anonym semaphore */
	sem_t mutex;    /* Anonym semaphore */
    Cola *cola;
} Estructura;

int main(void) {
    int aux = 0, j;
    int histograma[10];

    /* Abrimos el segmento de memoria compartida */
    int fd_shm = shm_open(SHM_NAME,
        O_RDWR,
        0);
    
    if (fd_shm == -1) {
        fprintf(stderr, "Error opening the shared memory segment\n");
        return EXIT_FAILURE;
    }

    /* Mapeamos el segmento de memoria compartida */
    Estructura *e = mmap(NULL, sizeof(*e),
        PROT_READ,
        MAP_SHARED,
        fd_shm,
        0);
    close(fd_shm);

    if (e == MAP_FAILED) {
        fprintf(stderr, "Error mapping the shared memory segment\n");
        return EXIT_FAILURE;
    }

    printf("Pointer to shared memory segment: %p\n", (void*)e);

    /* inicializamos el histograma a 0 */
    for (j = 0; j < 10; j++)
        histograma[j] = 0;

    while (aux != -1){
        printf("a  \n");
        fflush(stdout);
        sem_wait(&(e->full));
        printf("b\n");
        fflush(stdout);
        sem_wait(&(e->mutex));
	/* AQUI DA EL PUTO CORE DE LOS COJONES */
	/* Creo que es por donde inicializo la cola, pero
	   no se donde inicializarla sino (esta en el producer)*/
        printf("c\n");
        fflush(stdout);
        //aux = extraer(e->cola);
        printf("%d\n", aux);
        fflush(stdout);
        sem_post(&(e->mutex));
        sem_post(&(e->empty));

        histograma[aux]++;
        
    }

    for (j=0; j<10; j++){
        fprintf(stdout, "El número %d ha sido leído %d veces\n", j, histograma[j]);
    }

    /* Unmap the shared memory */
    munmap(e, sizeof(*e));
    return EXIT_SUCCESS;
}
