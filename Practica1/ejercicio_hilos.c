#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void* cube(void* arg){
  int* x=NULL;
  int* args=(int*)arg;

  sleep(args[1]);
  x=malloc(sizeof(int));
  if (x==NULL)
    return NULL;

  (*x)=args[0]*args[0]*args[0];
  return (void*)x;
}



int main(int argc, char *argv[]) {
  pthread_t* hilos=NULL;
  int numHilos, error=0;
  int args[2];
  int *retval=NULL;

  if (argc!=2 || argv[1]<=0)
    return EXIT_FAILURE;

  numHilos=atoi(argv[1]);
  hilos=malloc(numHilos*sizeof(pthread_t));

  for (int i=0; i<numHilos; i++){
    args[0]=i+1;
    args[1]=rand()%11;

    error = pthread_create(&hilos[i], NULL, cube, (void*)args);
  	if(error != 0){
  		fprintf(stderr, "pthread_create: %s\n", strerror(error));
  		exit(EXIT_FAILURE);
  	}

    error = pthread_join(hilos[i], (void*)&retval);
    if(error != 0){
      fprintf(stderr, "pthread_join: %s\n", strerror(error));
      exit(EXIT_FAILURE);
    }

    fprintf(stdout, "%d ", *retval);
  }


	printf("El programa %s termino correctamente \n", argv[0]);
	exit(EXIT_SUCCESS);
}
