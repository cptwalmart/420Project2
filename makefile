CFLAGS=-g -Wall -Werror
CC=mpicc
OBJS=main.o hash.o
output: ${OBJS}
	mpicc ${CFLAGS} -o output ${LDFLAGS} ${LIBS} $^
hashtest: hash_tests.c hash.c hash.h
	cc -g -o hashtest hash.c hash_tests.c
clean:
	rm *.o output
