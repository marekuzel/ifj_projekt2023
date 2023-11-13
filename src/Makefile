#Makefile
EXECUTABLE = scanner
CC = gcc
CFLAGS = -g -Wall --std=c11 -g --pedantic -Wextra

all: $(EXECUTABLE)

$(EXECUTABLE): scanner.o utils.o symtable.o
	$(CC) $(CFLAGS) -o $@ $^
test_table: symtable.o
%.o: %.c
	$(CC) $(CFLAGS) -c $^

.PHONY: clean, valgrind, test_table
clean:
	rm -f $(EXECUTABLE) *.o
valgrind: $(EXECUTABLE)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(EXECUTABLE)