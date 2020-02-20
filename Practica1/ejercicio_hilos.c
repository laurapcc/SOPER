/****************************************************************
* Nombre: ejercicio_hilos
* Autores: Jorge de Miguel y Laura de Paz
* Grupo de prácticas: 2202
* Fecha: 10/02/20
* Descripción: crea tantos hilos como se pase por argumentos
*              y espera un número aleatorio de segundos por
*              hilo.
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

typedef struct _Estructura{
  int random; /* numero aleatorio que se generara en cada hilo */
  int num; /* orden de creacion del hilo */
}Estructura;


/**
 * Nombre de la funcion: main
 * Parametros: arg: un puntero a void con los argumentos
 * Descripcion: cube calcula el cubo de un numero pasado como argumento
 *              en el puntero arg
 * Return: NULL si algo falla durante la ejecucion
 *         el cubo del numero pasado como argumento
 */
void* cube(void* arg){
  int* x=NULL;
  int n=((Estructura*)arg)->num;
  int wait=((Estructura*)arg)->random;

  sleep(wait);
  x=malloc(sizeof(int));
  if (x==NULL)
    return NULL;

  (*x)=n*n*n;
  return (void*)x;
}


/**
 * Nombre de la funcion: main
 * Parametros: 
 * Descripcion: funcion principal del programa
 * Return: EXIT_FAILURE si algo falla durante la ejecucion
 * 				 EXIT_SUCCES si se ejecuta correctamente
 */
int main(int argc, char *argv[]) {
  pthread_t* hilos=NULL;
  int numHilos, error=0;
  Estructura** args=NULL;
  int *retval=NULL;

  if (argc!=2 || argv[1]<=0)
    return EXIT_FAILURE;

  srand(time(NULL));

  numHilos=atoi(argv[1]);
  /* reservamos memoria para los hilos y estructura de cada hilo */
  hilos=malloc(numHilos*sizeof(pthread_t));
  args=malloc(numHilos*sizeof(Estructura*));
  for (int i=0; i<numHilos; i++){
    args[i]=malloc(sizeof(Estructura));
  }

  for (int i=0; i<numHilos; i++){
    /* se generan los argumentos que se pasaran a la funcion cube */
    args[i]->num=i+1;
    args[i]->random=rand()%11;

    /* se crea un hilo nuevo y se llama a la funcion cubo pasandole args[i] como argumentos */
    error = pthread_create(&hilos[i], NULL, cube, args[i]);
  	if(error != 0){
  		fprintf(stderr, "pthread_create: %s\n", strerror(error));
  		exit(EXIT_FAILURE);
  	}
  }

  for (int i=0; i<numHilos; i++){
    /* se espera a que termine el hilo i y recuperamos el valor de retorno de cube */
    error = pthread_join(hilos[i], (void*)&retval);
    if(error != 0){
      fprintf(stderr, "pthread_join: %s\n", strerror(error));
      exit(EXIT_FAILURE);
    }

    fprintf(stdout, "%d ", *retval);
    free(retval);
  }


	printf("El programa %s termino correctamente \n", argv[0]);
  for (int i=0; i<numHilos; i++){
    free(args[i]);
  }
  free(args);
  exit(EXIT_SUCCESS);
}
