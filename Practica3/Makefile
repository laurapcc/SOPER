CC = gcc
ANADIDOS = -g -lrt -lm -lpthread
CFLAGS = -Wall 
EXE = shm_producer shm_consumer

all : $(EXE)

.PHONY : clean
clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o cola.o
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -o $@ $@.o cola.o $(ANADIDOS)

cola.o: cola.c cola.h
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<

shm_producer.o: shm_producer.c cola.h
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $< $(ANADIDOS)

shm_consumer.o : shm_consumer.c cola.h
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $< $(ANADIDOS)


producer:
	@echo Ejecutando shm_producer
	@./shm_producer 3 1

consumer:
	@echo Ejecutando shm_consumer
	@./shm_consumer