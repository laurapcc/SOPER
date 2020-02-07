#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct _Estructura{
  int random;
  int num;
}Estructura;

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



int main(int argc, char *argv[]) {
  pthread_t* hilos=NULL;
  int numHilos, error=0;
  Estructura** args=NULL;
  int *retval=NULL;

  if (argc!=2 || argv[1]<=0)
    return EXIT_FAILURE;

  numHilos=atoi(argv[1]);
  hilos=malloc(numHilos*sizeof(pthread_t));
  args=malloc(numHilos*sizeof(Estructura*));
  for (int i=0; i<numHilos; i++){
    args[i]=malloc(sizeof(Estructura));
  }

  for (int i=0; i<numHilos; i++){
    args[i]->num=i+1;
    args[i]->random=rand()%11;

    error = pthread_create(&hilos[i], NULL, cube, args[i]);
  	if(error != 0){
  		fprintf(stderr, "pthread_create: %s\n", strerror(error));
  		exit(EXIT_FAILURE);
  	}
  }

  for (int i=0; i<numHilos; i++){
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
