/****************************************************************
* Nombre: cola
* Autores: Jorge de Miguel y Laura de Paz
* Grupo de prácticas: 2202
* Fecha: 12/04/2020
* Descripcion: TAD cola
****************************************************************/

#ifndef COLA_H
#define COLA_H

#define MAX_COLA 100

/* Estructura de la cola */
typedef struct _Cola{
    int array[MAX_COLA];
    int in;
    int out;
}Cola;

/*
*   Funcion encargada de inicializar la cola
*/
void cola_ini (Cola* c);

/*
*   Funcion encargada de insertar el elemento i en la cola c
*   Al ser una cola circular se actualiza el elemento in con modulo n
*/
void insertar(Cola* c, int i);

/*
*	Funcion encargada de extraer el elemento indicado por out de la cola.
*	Lo devuelve y actualiza out modulo n
*/
int extraer(Cola* c);

#endif
