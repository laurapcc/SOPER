#include <stdio.h>
#include <stdlib.h>
#include "cola.h"


void cola_ini(Cola* c){
    c->in = 0;
    c->out = 0;
}

void insertar(Cola* c, int i){
    c->array[c->in] = i;
    c->in = (c->in+1)%MAX_COLA;
}

int extraer(Cola* c){
    if (c->in == c->out)
        return -2;
    int output = c->array[c->out];
    c->out = (c->out+1)%MAX_COLA;

    return output;
}
