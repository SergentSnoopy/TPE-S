
CC=gcc
CFLAGS=-W -Wall -g -std=c99 -pthread

EXEC=main
all: $(EXEC)

%.C: %.o
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -rf *.o *.run
