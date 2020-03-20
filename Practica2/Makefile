CC = gcc -pthread
CFLAGS = -Wall
EXE = ejercicio_kill

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

ejercicio_kill.o: ejercicio_kill.c
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<

ejercicio_prottemp.o : ejercicio_prottemp.c
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<

ejercicio_prottemp_mejorado.o : ejercicio_prottemp_mejorado.c
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<


# de ejemplo 
prottemp:
	@echo Ejecutando ejercicio_prottemp
	@./ejercicio_prottemp 10 4

prottemp_mejorado:
	@echo Ejecutando ejercicio_prottemp_mejorado
	@./ejercicio_prottemp_mejorado 10 4


