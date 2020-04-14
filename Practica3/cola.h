#ifndef COLA_H
#define COLA_H

#define MAX_COLA 100

//typedef struct _Cola Cola;
typedef struct _Cola{
    int array[MAX_COLA];
    int in;
    int out;
}Cola;

Cola* cola_ini ();

void insertar(Cola* c, int i);

int extraer(Cola* c);

#endif