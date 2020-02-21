#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>


int main(int argc, char* argv[]) {
    if (argc != 3){
        printf("El programa recibe exactamente 2 argumentos\n");
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-'){
        printf("Se debe ejecutar el programa con el siguiente formato:\n");
        printf("./ejercicio_kill -<signal> <pid>\n");
        exit(EXIT_FAILURE);
    }

    char *aux = argv[1];
    int signal = atoi(++aux);
    pid_t pid = (pid_t)atoi(argv[2]);

    if (kill(pid, signal) == -1){
        perror("kill");
        exit(EXIT_FAILURE);
    }
    
    printf("El programa funciono correctamente\n");
    exit(EXIT_SUCCESS);
}
