#Makefile
EXECUTABLE = scanner
CC = gcc
CFLAGS = -g -Wall --std=c11 -g --pedantic -Wextra

all: $(EXECUTABLE)

$(EXECUTABLE): scanner.o utils.o symtable.o code_gen.o bu_analysis.o prec_table.o parser.o syntax_rules.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean, valgrind, test_code_gen, test_scanner
clean:
	rm -f $(EXECUTABLE) *.o test_code_gen bu
valgrind: $(EXECUTABLE)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(EXECUTABLE)

test_code_gen: code_gen.o symtable.o code_gen_test.o utils.o
	$(CC) $(CFLAGS) -o $@ $^
	valgrind --leak-check=full --track-origins=yes -s ./test_code_gen > ../interpreter/test_code.txt
	../interpreter/ic23int ../interpreter/test_code.txt

test_scanner: scanner.o utils.o
	$(CC) $(CFLAGS) -o $@ $^
	./test_scanner < test.txt

test_parser: parser.o symtable.o utils.o
	$(CC) $(CFLAGS) -o $@ $^ -RUN_TESTS
	../test_parser < test.txt