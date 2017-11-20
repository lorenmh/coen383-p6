CC=gcc
CFLAGS=-std=c99
CMD=$(CC) $(CFLAGS)

COMPILE=$(CMD) -g -c 

FILES=pipes.o 

all: pipes.o
	$(CMD) $(FILES) -g -o app.bin

clean:
	rm *.o app.bin

pipes.o: pipes.c
	$(COMPILE) pipes.c
