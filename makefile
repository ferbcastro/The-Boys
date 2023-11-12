CFLAGS = -Wall -Wextra -g  # flags de compilacao
LDFLAGS = -lm

CC = gcc 
     
all: theboys

theboys: theboys.o conjunto.o  lef.o fila.o
	$(CC) -o theboys theboys.o conjunto.o  lef.o fila.o $(LDFLAGS)

conjunto.o: conjunto.c
	$(CC) -c $(CFLAGS) conjunto.c

lef.o: lef.c
	$(CC) -c $(CFLAGS) lef.c

fila.o: fila.c
	$(CC) -c $(CFLAGS) fila.c

theboys.o: theboys.c
	$(CC) -c $(CFLAGS) theboys.c

clean:
	rm -f *.o theboys 


