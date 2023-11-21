## SRB
## Makefile for unit tests

L = ../include
LIBS = pcre2-8

CC = clang
CFLAGS = -Wall -I$L

EXE = test_*

TESTS = $T/test_number $T/test_parser

T = unittests

BISON = /opt/homebrew/opt/bison/bin/bison
FLEX = /opt/homebrew/opt/flex/bin/flex

all: test_parser

test_parser: parser.tab.c lexer.c
	cc -Wall $^ -ll -o $@

parser.tab.c: parser.y
	$(BISON) -d $^

lexer.c: lexer.l
	$(FLEX) -o $@ $^
 	
test:
	make -C $T test
	
clean:
	rm -f *.o *.tab.* lexer.c test_parser
	make -C $T clean
