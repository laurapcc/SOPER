#########################################
CC = gcc
ANADIDOS = -g -lrt -lm -lpthread
CFLAGS = -Wall 
EXE = shm_concurrence shm_concurrence_solved shm_producer shm_consumer shm_producer_file shm_consumer_file mq_injector mq_workers_pool
#########################################
OBJECTSPROD = shm_producer.o cola.o
OBJECTSCONS = shm_consumer.o cola.o
OBJECTSPRODFILE = shm_producer_file.o cola.o
OBJECTSCONSFILE = shm_consumer_file.o cola.o
#########################################

all : $(EXE)

.PHONY : clean
clean :
	rm -f *.o core *.h.gch  $(EXE)


# EJERCICIO 3
shm_concurrence: shm_concurrence.o
	$(CC) $(CFLAGS) -o $@ $^ $(ANADIDOS)

shm_concurrence.o: shm_concurrence.c
	$(CC) $(CFLAGS) -c $< $^ $(ANADIDOS)

shm_concurrence_solved: shm_concurrence_solved.o
	$(CC) $(CFLAGS) -o $@ $^ $(ANADIDOS)

shm_concurrence_solved.o: shm_concurrence_solved.c
	$(CC) $(CFLAGS) -c $< $^ $(ANADIDOS)


# EJERCICIO 4
shm_producer: $(OBJECTSPROD)
	$(CC) $(CFLAGS) -o $@ $^ $(ANADIDOS)

shm_consumer: $(OBJECTSCONS)
	$(CC) $(CFLAGS) -o $@ $^ $(ANADIDOS)

shm_producer.o: shm_producer.c cola.h
	$(CC) $(CFLAGS) -c $< $^ $(ANADIDOS)

shm_consumer.o: shm_consumer.c cola.h
	$(CC) $(CFLAGS) -c $< $^ $(ANADIDOS)

cola.o: cola.c cola.h
	$(CC) $(CFLAGS) -c $< $(ANADIDOS)


# EJERCICIO 4B OPCIONAL
shm_producer_file: $(OBJECTSPROD)
	$(CC) $(CFLAGS) -o $@ $^ $(ANADIDOS)

shm_consumer_file: $(OBJECTSCONS)
	$(CC) $(CFLAGS) -o $@ $^ $(ANADIDOS)

shm_producer_file.o: shm_producer_file.c cola.h
	$(CC) $(CFLAGS) -c $< $^ $(ANADIDOS)

shm_consumer_file.o: shm_consumer_file.c cola.h
	$(CC) $(CFLAGS) -c $< $^ $(ANADIDOS)


# EJERCICIO 6
mq_injector: mq_injector.o
	$(CC) $(CFLAGS) -o $@ $^ $(ANADIDOS)

mq_injector.o: mq_injector.c
	$(CC) $(CFLAGS) -c $< $^ $(ANADIDOS)

mq_workers_pool: mq_workers_pool.o
	$(CC) $(CFLAGS) -o $@ $^ $(ANADIDOS)

mq_workers_pool.o: mq_workers_pool.c
	$(CC) $(CFLAGS) -c $< $^ $(ANADIDOS)



########### PARA PRUEBA ###########

# EJERCICIO 3
concurrence:
	@echo Ejecutando shm_concurrence
	@./shm_concurrence 5 5

concurrence_solved:
	@echo Ejecutando shm_concurrence_solved
	@./shm_concurrence_solved 5 5


# EJERCICIO 4
producer:
	@echo Ejecutando shm_producer
	@./shm_producer 12 0

consumer:
	@echo Ejecutando shm_consumer
	@./shm_consumer


# EJERCICIO 4B
producer_file:
	@echo Ejecutando shm_producer_file
	@./shm_producer_file 12 0

consumer_file:
	@echo Ejecutando shm_consumer_file
	@./shm_consumer_file


# EJERCICIO 6
injector:
	@echo Ejecutando mq_injector
	@./mq_injector prueba.txt /cola_ej6

workers_pool:
	@echo Ejecutando mq_workers_pool
	@./mq_workers_pool 5 /cola_ej6 c