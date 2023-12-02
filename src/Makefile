#Makefile
EXECUTABLE = scanner
CC = gcc
CFLAGS = -g -Wall --std=c11 -g --pedantic -Wextra
LEAKS = "All heap blocks were freed -- no leaks are possible"
ERRORS = "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)"

all: $(EXECUTABLE)

$(EXECUTABLE): scanner.o utils.o symtable.o bu_analysis.o prec_table.o code_gen.o parser.o syntax_rules.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean, valgrind, test_code_gen, test_scanner 
clean:
	rm -f $(EXECUTABLE) tests/*.o test_code_gen bu test_scanner *.o
valgrind: $(EXECUTABLE)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(EXECUTABLE)

test_code_gen: code_gen.o symtable.o ./tests/code_gen_test.o utils.o
	$(CC) $(CFLAGS) -o $@ $^
	valgrind --leak-check=full --track-origins=yes -s --log-file=./tests/test_out/code_gen_test_valgrind.out ./test_code_gen > ../interpreter/test_code.txt
	../interpreter/ic23int ../interpreter/test_code.txt > ./tests/test_out/code_gen_test.out
	diff ./tests/test_out/code_gen_test.exp_out ./tests/test_out/code_gen_test.out && echo "code output is correct!"
	grep -silent $(LEAKS) ./tests/test_out/code_gen_test_valgrind.out && grep -silent $(ERRORS) ./tests/test_out/code_gen_test_valgrind.out  && echo "valgrind ok!"

test_scanner: scanner.o utils.o ./tests/scanner_test.o	
	$(CC) $(CFLAGS) -o $@ $^
	valgrind -s --log-file=./tests/test_out/scanner_test_valgrind.out ./test_scanner < ./tests/test_in/scanner_test_in.txt >./tests/test_out/scanner_test.out
	diff ./tests/test_out/scanner_test.exp_out ./tests/test_out/scanner_test.out && echo "token output if correct"
	grep --silent $(ERRORS) ./tests/test_out/scanner_test_valgrind.out  && echo "valgrind ok!"

test_parser: parser.o symtable.o utils.o ./tests/parser_test.o syntax_rules.o prec_table.o bu_analysis.o scanner.o code_gen.o
	$(CC) $(CFLAGS) -o $@ $^
	../test_parser 1  < /test_in/parser_test_1.txt > test_out/test_parser.out