#ifndef _SORT_H
#define _SORT_H

#include <mqueue.h>
#include <semaphore.h>
#include <sys/types.h>
#include "global.h"

/* Constants. */
#define MAX_DATA 100000
#define MAX_LEVELS 10
#define MAX_PARTS 512
#define MAX_STRING 1024

#define PLOT_PERIOD 1
#define NO_MID -1

#define SHM_NAME "/shm_proyecto"
#define MQ_NAME "/mq_proyecto"

/* Pipes */
#define READ_PIPE 0
#define WRITE_PIPE 1

/* Ratio de refresco de ilustrador */
#define SECS 1

/* Variable global */
int pids[MAX_PARTS];
int global_n_proc;

int pipe_trab2ilust[2];
int pipe_ilust2trab[2];

int estado[3];
int alarma;

/* Type definitions. */

/* Completed flag for the tasks. */
typedef enum {
    INCOMPLETE,
    SENT,
    PROCESSING,
    COMPLETED
} Completed;

/* Task. */
typedef struct {
    Completed completed;
    int ini;
    int mid;
    int end;
    sem_t mutex;
} Task;

/* Structure for the sorting problem. */
typedef struct{
    Task tasks[MAX_LEVELS][MAX_PARTS];
    int data[MAX_DATA];
    int delay;
    int n_elements;
    int n_levels;
    int n_processes;
    pid_t ppid;
} Sort;

/* Prototypes. */

/**
 * Sorts an array using bubble-sort.
 * @method bubble_sort
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  vector      Array with the data.
 * @param  n_elements  Number of elements in the array.
 * @param  delay       Delay for the algorithm.
 * @return             ERROR in case of error, OK otherwise.
 */
Status bubble_sort(int *vector, int n_elements, int delay);

/**
 * Merges two ordered parts of an array keeping the global order.
 * @method merge
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  vector     Array with the data.
 * @param  middle     Division between the first and second parts.
 * @param  n_elements Number of elements in the array.
 * @param  delay      Delay for the algorithm.
 * @return            ERROR in case of error, OK otherwise.
 */
Status merge(int *vector, int middle, int n_elements, int delay);

/**
 * Computes the number of parts (division) for a certain level of the sorting
 * algorithm.
 * @method get_number_parts
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  level            Level of the algorithm.
 * @param  n_levels         Total number of levels in the algorithm.
 * @return                  Number of parts in the level.
 */
int get_number_parts(int level, int n_levels);

/**
 * Initializes the sort structure.
 * @method init_sort
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  file_name   File with the data.
 * @param  sort        Pointer to the sort structure.
 * @param  n_levels    Total number of levels in the algorithm.
 * @param  n_processes Number of processes.
 * @param  delay       Delay for the algorithm.
 * @return             ERROR in case of error, OK otherwise.
 */
Status init_sort(char *file_name, Sort *sort, int n_levels, int n_processes, int delay);

/**
 * Checks if a task is ready to be solved.
 * @method check_task_ready
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  sort             Pointer to the sort structure.
 * @param  level            Level of the algorithm.
 * @param  part             Part inside the level.
 * @return                  FALSE if it is not ready, TRUE otherwise.
 */
Bool check_task_ready(Sort *sort, int level, int part);

/**
 * Checks if all the tasks in a level are completed
 * @method check_nivel_completed
 * @date   2020-04-26
 * @author Jorge de Miguel y Laura de Paz
 * @param  sort             Pointer to the sort structure.
 * @param  level            Level of the algorithm.
 * @return                  FALSE if it at least one task is not completed, TRUE otherwise.
 */
Bool check_nivel_completed(Sort* sort, int level);

/**
 * Solves a single task of the sorting algorithm.
 * @method solve_task
 * @date   2020-04-09
 * @author Jorge de Miguel y Laura de Paz
 * @param  sort       Puntero a la estructura Sort
 * @param  task       Tarea a realizar
 * @return            ERROR in case of error, OK otherwise.
 */
Status solve_task(Sort* sort, Task* task);

/**
 * Codigo principal del hijo que se encargara de realizar las diferentes tareas.
 * @method trabajador
 * @date   2020-04-26
 * @author Jorge de Miguel y Laura de Paz
 * @param  s                Puntero a la estructura sort en memoria compartida
 * @return                  ERROR in case of error, OK otherwise.
 */
Status trabajador(Sort* sort);

/**
 * Codigo principal del ilustrador.
 * @method ilustrador
 * @date   2020-04-28
 * @author Jorge de Miguel y Laura de Paz
 * @param  s                Puntero a la estructura sort en memoria compartida
 * @return                  ERROR in case of error, OK otherwise.
 */
Status ilustrador(Sort* sort);

/**
 * Solves a sorting problem using multiple processes.
 * @method sort_multiple_processes
 * @date   2020-04-26
 * @author Jorge de Miguel y Laura de Paz
 * @param  s                Puntero a la estructura sort en memoria compartida
 * @return                  ERROR in case of error, OK otherwise.
 */
Status sort_multiple_processes(Sort* s);

#endif
