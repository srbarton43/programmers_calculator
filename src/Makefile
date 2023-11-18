## SRB
## Makefile for unit tests

L = ../include
LIBS = pcre2-8

CC = clang
CFLAGS = -Wall -I$L

EXE = test_*

TESTS = $T/test_number $T/test_parser

T = unittests

all:

# $T/test_number: number.c $T/utils.o
# 	$(CC) $(CFLAGS) -DUNIT_TEST -I../include number.c $T/utils.o -o $@
# 
# $T/utils.o: ../include/utils.h
# 	$(CC) $(CFLAGS) -c utils.c -o $@
# 
# $T/test_parser: parser.c
# 	$(CC) $(CFLAGS) -DUNIT_TEST -l$(LIBS) parser.c -o $@
# 	
test:
	make -C $T test
	
clean:
	rm -f *.o
	make -C $T clean
