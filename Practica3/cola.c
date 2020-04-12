#include <stdio.h>
#include <stdlib.h>
#include "cola.h"


typedef struct _Cola{
    int N;
    int *array;
    int in;
    int out;
}Cola;


Cola* cola_ini(int N){
    Cola *c = NULL;

    c = malloc(sizeof(Cola));
    if (c==NULL)
        return NULL;

    c->array = malloc((N)*sizeof(int));
    if (c->array==NULL){
        free(c);
        return NULL;
    }

    c->N = N;
    c->in = 0;
    c->out = 0;

    return c;
}

void insertar(Cola* c, int i){
    if (c->in==c->out)
        return;

    c->array[c->in] = i;
    c->in = (c->in+1)%(c->N);
}

int extraer(Cola* c){
    int output;

    if (c->in==c->out)
        return -1;
    
    printf("dentro\n");
    fflush(stdout);
    output = c->array[c->out];
    printf("dentro 2\n");
    fflush(stdout);
    c->out = (c->out+1)%(c->N);

    return output;
}