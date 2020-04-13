#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define DOSKB 2048

void manejador (int sig) {
    return;
}

int main(int argc, char *argv[]) {
    char* mq_name;
    int n;
    char key;
    pid_t pid;
    struct sigaction act;
    sigset_t set, set2;

    if (argc!=4){
        fprintf(stdout, "Error en el numero de argumentos");
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);
    mq_name = argv[2];
    key = (char)argv[3][0];
    pid_t array_pid[n];

    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = DOSKB*sizeof(char)
    };

    mqd_t queue = mq_open(mq_name,
        O_CREAT | O_RDONLY, /* This process is only going to send messages */
        S_IRUSR | S_IWUSR, /* The user can read and write */
        &attributes);

    if(queue == (mqd_t)-1) {
        fprintf(stderr, "Error opening the queue\n");
        return EXIT_FAILURE;
    }

    /* Bloqueamos la señal SIGUSR2 para evitar que llegue antes de tiempo */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &set, NULL);

    /* manejar SIGUSR2 */
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    act.sa_handler = manejador;
    if (sigaction(SIGUSR2, &act, NULL) < 0) {
    	fprintf(stderr, "Error in sigaction\n");
		mq_unlink(mq_name);
    	return EXIT_FAILURE;
    }

    /* crear n hijos */
	for (int i = 0; i < n; i++){
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

    if (pid){   /* padre */
        sigfillset(&set2);
        sigdelset(&set2, SIGUSR2);
        sigsuspend(&set2);

        for (int i=0; i<n; i++){
            if (kill(array_pid[i], SIGTERM) < 0){
				fprintf(stderr, "Error en proceso con pid = %jd al enviar SIGTERM\n", (intmax_t)array_pid[i]);
				return EXIT_FAILURE;
			}
        }
        mq_close(queue);
        return EXIT_SUCCESS;
    }
    else{   /* hijos */
        /* manejar SIGTERM */
        sigemptyset(&(act.sa_mask));
        act.sa_flags = 0;
        act.sa_handler = manejador;
        if (sigaction(SIGTERM, &act, NULL) < 0) {
            fprintf(stderr, "Error in sigaction\n");
            mq_unlink(mq_name);
            return EXIT_FAILURE;
        }

        char msg[DOSKB];
        int m=0;
        int counter = 0;
        pid_t ppid = getppid();

        while (1){
            if (mq_receive(queue, msg, sizeof(msg), NULL) == -1) {
                if (errno==EINTR){ // interrupted by a signal handler
                    fprintf(stdout, "%jd: mensajes leídos=%d; caracteres %c encontrados=%d\n", (intmax_t)getpid(), m, key, counter);
                    fflush(stdout);
                    mq_close(queue);
                    return EXIT_SUCCESS;
                }
                else{
                    fprintf(stderr, "Error receiving message\n");
                    return EXIT_FAILURE;
                }
            }
            m++;
            if (strcmp(msg, "Final\0")==0){
                if (kill(ppid, SIGUSR2) < 0){
                    fprintf(stderr, "Error en proceso con pid = %jd al enviar SIGUSR2\n", (intmax_t)ppid);
                    return EXIT_FAILURE;
			    }
            }
            for (int i=0; i<strlen(msg); i++){
                if (msg[i]==key) counter++;
            }
        }
    }
}
