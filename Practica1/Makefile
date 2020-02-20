CC = gcc -pthread
CFLAGS = -Wall
EXE = ejercicio_hilos ejercicio_arbol ejercicio_shell ejercicio_pipes

all : $(EXE)

.PHONY : clean
clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -o $@ $@.o

ejercicio_hilos.o : ejercicio_hilos.c
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<

ejercicio_arbol.o : ejercicio_arbol.c
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<

ejercicio_shell.o : ejercicio_shell.c
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<

ejercicio_pipes.o : ejercicio_pipes.c
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<


hilos:
	@echo Ejecutando ejercicio_hilos
	@./ejercicio_hilos 5

arbol:
	@echo Ejecutando ejercicio_arbol
	@./ejercicio_arbol

shell:
	@echo Ejecutando ejercicio_shell
	@./ejercicio_shell

pipes:
	@echo Ejecutando ejercicio_pipes
	@./ejercicio_pipes
