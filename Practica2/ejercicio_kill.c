/****************************************************************
* Nombre: ejercicio_kill
* Autores: Jorge de Miguel y Laura de Paz
* Grupo de prácticas: 2202
* Fecha: 21/02/2020
* Descripcion: ejecuta la funcion kill con el PID y el identificador
               numerico de la signal pasados como argumentos
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>


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
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-'){
        printf("Se debe ejecutar el programa con el siguiente formato:\n");
        printf("./ejercicio_kill -<signal> <pid>\n");
        exit(EXIT_FAILURE);
    }

    char *aux = argv[1];
    int signal = atoi(++aux); /* omite el guion delante del <signal> */
    pid_t pid = (pid_t)atoi(argv[2]);

    if (kill(pid, signal) == -1){
        perror("kill");
        exit(EXIT_FAILURE);
    }
    
    printf("El programa funciono correctamente\n");
    exit(EXIT_SUCCESS);
}
