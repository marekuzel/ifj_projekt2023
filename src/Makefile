#Makefile
CC=gcc
CFLAGS=-std=c11 -pedantic -Wall -Wextra -g

all: scanner.c
	$(CC) $(CFLAGS) $^ -o $@


scanner.o: scanner.c scanner.h str.h error.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o *.d scanner#Makefile
CC=gcc
CFLAGS=-std=c11 -pedantic -Wall -Wextra -g

all: scanner.c
	$(CC) $(CFLAGS) $^ -o $@


scanner.o: scanner.c scanner.h str.h error.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o *.d scanner