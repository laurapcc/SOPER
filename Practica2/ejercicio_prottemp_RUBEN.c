#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void manejador_SIGNAL(int sig) {
    return;
}

int main(int argc, char** argv) {
    struct sigaction act;
    sigset_t mask;
    int N = atoi(argv[1]);
    int T = atoi(argv[2]);
    pid_t pid;
    pid_t cpid[N];
    int i, suma;

    for (i=0; i<N; i++) {
        if (!(pid = fork()))
            break;
        else
            cpid[i] = pid;
    }

    if (pid == 0) {
        sigemptyset(&(act.sa_mask));
        act.sa_flags = 0;
        act.sa_handler = manejador_SIGNAL;
        if (sigaction(SIGTERM, &act, NULL) < 0) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        sigfillset(&mask);
        sigdelset(&mask, SIGTERM);

        for (i=1, suma=0; i<getpid()/10; i++) {
            suma += i;
        }
        fprintf(stdout, "(PID: %d) = %d\n", getpid(), suma);

        kill(getppid(), 12);
        sigsuspend(&mask);

        fprintf(stdout, "Finalizado %d\n", getpid());
        exit(EXIT_SUCCESS);
    }
    else {
        sigemptyset(&(act.sa_mask));
        act.sa_flags = 0;
        act.sa_handler = manejador_SIGNAL;
        if (sigaction(SIGALRM, &act, NULL) < 0) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        if (sigaction(SIGUSR2, &act, NULL) < 0) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        sigfillset(&mask);
        sigdelset(&mask, SIGALRM);

        alarm(T);
        sigsuspend(&mask);

        for (i=0; i<N; i++) {
            kill(cpid[i], 15);
        }

        fprintf(stdout, "Finalizado Padre\n");
        for (i=0; i<N; i++) {
            wait(NULL);
        }
        exit(EXIT_SUCCESS);
    }
}
