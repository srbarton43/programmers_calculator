L = ../../include

CC = clang
CFLAGS = -Wall -I$L -DUNIT_TEST

OBJS = number.o utils.o

unittest:	unittest.c $(OBJS)
	$(CC) $(CFLAGS)  $^ -o $@

number.o: ../number.c $L/number.h $L/utils.h
	$(CC) $(CFLAGS) -c ../number.c -o $@

utils.o: ../utils.c $L/utils.h
	$(CC) $(CFLAGS) -c ../utils.c -o $@

test: unittest
	./unittest

clean:
	rm -f *.o test_* unittest
