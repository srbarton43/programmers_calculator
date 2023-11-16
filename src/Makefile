## SRB
## Makefile for unit tests

L = ../include
LIBS = pcre2-8

CC = clang
CFLAGS = -Wall -pedantic -I$L

EXE = test_*

TESTS = test_number test_parser

all: $(TESTS)

test_number: number.c utils.o
	$(CC) $(CFLAGS) -DUNIT_TEST -I../include number.c utils.o -o $@

utils.o: ../include/utils.h

test_parser: parser.c
	$(CC) $(CFLAGS) -DUNIT_TEST -l$(LIBS) parser.c -o $@
	
test: $(TESTS)
	./test_parser
	./test_number

clean:
	rm -f *.o $(EXE)
