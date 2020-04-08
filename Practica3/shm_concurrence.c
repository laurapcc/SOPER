#include <errno.h>
#include <fcntl.h>
#include <math.h>
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
} ClientLog;

ClientLog *shm_struct;

void manejador (int sig) {
	printf("   padre en manejador!!!\n");
	if (sig == SIGUSR1) {
		printf ("Log %ld: Pid %d: %s\n",shm_struct->logid, shm_struct->processid, shm_struct->logtext);
	}
	printf("   padre sale de manejador...\n");
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

	/* crear n hijos */
	for (i = 0; i < n; i++){
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
		printf("padre\n");

		/* crear memoria compartida */
		int fd_shm = shm_open(SHM_NAME,
        			O_RDWR | O_CREAT | O_EXCL, 
        			S_IRUSR | S_IWUSR);

		if (fd_shm == -1) {
			fprintf(stderr, "Error creating the shared memory segment\n");
			return EXIT_FAILURE;
    	}

		/* Truncar el tamano de la memoria compartida */
		if (ftruncate(fd_shm, sizeof(ClientLog)) == -1) {
			fprintf(stderr, "Error resizing the shared memory segment\n");
			shm_unlink(SHM_NAME);
			return EXIT_FAILURE;
		}

		/* Mapear el segmento de memoria */
		ClientLog *my_struct = mmap(NULL, sizeof(ClientLog), PROT_READ | PROT_WRITE,
									MAP_SHARED, fd_shm, 0);
		close(fd_shm);

		if (my_struct == MAP_FAILED) {
			fprintf(stderr, "Error mapping the shared memory segment\n");
			shm_unlink(SHM_NAME);
			return ret;
		}

		/* Inicializar logid a -1 */ /* NO SE ESTO ESTA BIEN AQUI */
		my_struct->logid = -1;

		/* manejar SIGUSR1 */
        sigemptyset(&(act.sa_mask));
        act.sa_flags = 0;
        act.sa_handler = manejador;
        if (sigaction(SIGUSR1, &act, NULL) < 0) {
            fprintf(stderr, "Error in sigaction\n");
			shm_unlink(SHM_NAME);
            return ret;
        }

        /* suspender proceso hasta que reciba SIGUSR1 */
        sigfillset(&set);
        sigdelset(&set, SIGUSR1);
        sigsuspend(&set);

		/* espera a que acaben todo los hijos */
		for (i = 0; i < n; i++)
			wait(NULL);

		munmap(my_struct, sizeof(ClientLog));
    	shm_unlink(SHM_NAME);
		
		return EXIT_SUCCESS;
	}
	//else

	printf("hijo\n");

	for (i = 0; i < m; i++){
		usleep((rand()%801 + 100) * 1000); /* random entre 100 y 900 ms */

		/* abrir la memoria compartida */
		int fd_shm = shm_open(SHM_NAME, O_RDWR, 0); 

		if (fd_shm == -1) {
			fprintf(stderr, "Error opening the shared memory segment (pid = %jd)\n", (intmax_t)getpid());
			perror("open memory");
			return EXIT_FAILURE;
		}

		/* Mapear el segmento de memoria */
		ClientLog *my_struct = mmap(NULL, sizeof(ClientLog), PROT_WRITE, MAP_SHARED, fd_shm, 0);
		close(fd_shm);

		if (my_struct == MAP_FAILED) {
			fprintf(stderr, "Error mapping the shared memory segment\n");
			return EXIT_FAILURE;
		}

		/*
		Rellenará la información de la estructura ClientLog para generar una
		nueva lı́nea de log, de manera que processid sea igual al PID del
		proceso, se incremente el campo logid (clinealogid = clinealogid + 1),
		y logtext contenga el mensaje "Soy el proceso <PID> a las HH:MM:SS:mmm"
		*/


		/* Escribir en la estructura compartida */
		printf("A escibir (pid = %jd)\n", (intmax_t)getpid());
		my_struct->processid = getpid();
		my_struct->logid++;
		char buf[MAX_MSG]; 
		getMilClock(buf);
		memcpy(my_struct->logtext, buf, MAX_MSG);
		printf("Termina escibir (pid = %jd)\n", (intmax_t)getpid());


		/* enviar senal SIGUSR1 a padre */
		if (kill(getppid(), SIGUSR1) < 0){
			fprintf(stderr, "Error en proceso con pid = %jd al enviar SIGUSR1\n", (intmax_t)getpid());
			/* liberar algo de shm? en el ejmplo de reader no lo hace */
			return ret;
		}

		/* Unmap the shared memory */
		munmap(my_struct, sizeof(ClientLog));
	}
	printf("hijo termina guay\n");


	return EXIT_SUCCESS;
}
