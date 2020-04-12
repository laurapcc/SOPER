#ifndef COLA_H
#define COLA_H


typedef struct _Cola Cola;

Cola* cola_ini (int N);

void insertar(Cola* c, int i);

int extraer(Cola* c);

#endif