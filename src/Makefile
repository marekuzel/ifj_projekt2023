#Makefile
EXECUTABLE = scanner
CC = gcc
CFLAGS = -g -Wall --std=c11 -g --pedantic -Wextra

all: $(EXECUTABLE)

$(EXECUTABLE): scanner.o utils.o symtable.o code_gen.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean, valgrind, test_table
clean:
	rm -f $(EXECUTABLE) *.o
valgrind: $(EXECUTABLE)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(EXECUTABLE)