#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SHM_NAME "/shm_eje3"
#define MAX_MSG 2000

static void getMilClock(char *buf) {
    int millisec;
	char aux[100];
    struct tm* tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);
	millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
    if (millisec>=1000) { // Allow for rounding up to nearest second
        millisec -=1000;
        tv.tv_sec++;
    }
    tm_info = localtime(&tv.tv_sec);
    strftime(aux, 10, "%H:%M:%S", tm_info);
	sprintf(buf, "%s.%03d", aux, millisec);
}

typedef struct {
	pid_t processid;       /* Logger process PID */
	long logid;            /* Id of current log line */
	char logtext[MAX_MSG]; /* Log text */
	sem_t mutex;		   /* Anonym semaphore */
} ClientLog;

ClientLog *shm_struct;

void manejador (int sig) {
	if (sig == SIGUSR1) {
		printf ("Log %ld: Pid %d: %s\n",shm_struct->logid, shm_struct->processid, shm_struct->logtext);
	}
}

int main(int argc, char *argv[]) {
	int n, m, i;
	int ret = EXIT_FAILURE;
	pid_t pid;
	pid_t array_pid[n];
	struct sigaction act;
    sigset_t set;

	if (argc < 3) {
		fprintf(stderr,"usage: %s <n_process> <n_logs> \n",argv[0]);
		return ret;
	}

	n = atoi(argv[1]);
	m = atoi(argv[2]);

	/* crear memoria compartida */
	int fd_shm = shm_open(SHM_NAME,
				O_RDWR | O_CREAT | O_EXCL, 
				S_IRUSR | S_IWUSR);

	if (fd_shm == -1) {
		fprintf(stderr, "Error creating the shared memory segment\n");
		return EXIT_FAILURE;
	}

	/* Truncar el tamano de la memoria compartida */
	if (ftruncate(fd_shm, sizeof(*shm_struct)) == -1) {
		fprintf(stderr, "Error resizing the shared memory segment\n");
		shm_unlink(SHM_NAME);
		return EXIT_FAILURE;
	}

	/* Mapear el segmento de memoria */
	shm_struct = mmap(NULL, sizeof(*shm_struct), PROT_READ | PROT_WRITE,
								MAP_SHARED, fd_shm, 0);
	close(fd_shm);

	if (shm_struct == MAP_FAILED) {
		fprintf(stderr, "Error mapping the shared memory segment\n");
		shm_unlink(SHM_NAME);
		return ret;
	}

	/* Inicializar la estructura: logid y el semaforo */
	shm_struct->logid = -1;
	if (sem_init(&(shm_struct->mutex), 1, 0) == -1){ // 1: para poder compartir entre procesos y 1: valor inicial
		fprintf(stderr, "Error creating semaphore\n");
		shm_unlink(SHM_NAME);
		return ret;
	}

	/* manejar SIGUSR1 */
	sigemptyset(&(act.sa_mask));
	act.sa_flags = 0;
	act.sa_handler = manejador;
	if (sigaction(SIGUSR1, &act, NULL) < 0) {
		fprintf(stderr, "Error in sigaction\n");
		shm_unlink(SHM_NAME);
		return ret;
	}

	sigfillset(&set);
	sigdelset(&set, SIGUSR1);
	sigdelset(&set, SIGINT);

	/* crear n hijos */
	for (i = 0; i < n; i++){
        pid = fork();
        if (pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) /* hijo */
            break;
    }

	if (pid){ /* padre */
		do{
			sem_post(&(shm_struct->mutex));
			sigsuspend(&set);
		}while(shm_struct->logid < m*n - 1);

		/* espera a que acaben todo los hijos */
		for (i = 0; i < n; i++)
			wait(NULL);

		munmap(shm_struct, sizeof(*shm_struct));
    	shm_unlink(SHM_NAME);
		sem_destroy(&(shm_struct->mutex));
		return EXIT_SUCCESS;
	}

	else{
		for (i = 0; i < m; i++){
			usleep((rand()%801 + 100) * 1000); /* random entre 100 y 900 ms */

			/* Escribir en la estructura compartida */
			sem_wait(&(shm_struct->mutex));
			shm_struct->processid = getpid();
			shm_struct->logid++;
			char buf[MAX_MSG]; 
			getMilClock(buf);
			memcpy(shm_struct->logtext, buf, MAX_MSG);
			//sem_post(&(shm_struct->mutex));

			/* enviar senal SIGUSR1 a padre */
			if (kill(getppid(), SIGUSR1) < 0){
				fprintf(stderr, "Error en proceso con pid = %jd al enviar SIGUSR1\n", (intmax_t)getpid());
				return ret;
			}
		}
		/* Unmap the shared memory */
		munmap(shm_struct, sizeof(*shm_struct));

	}



	return EXIT_SUCCESS;
}
