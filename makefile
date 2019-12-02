CFLAGS=-g -Wall #-Werror
CC=mpicc
OBJS=main.o hash.o matops.o
output: ${OBJS}
	mpicc ${CFLAGS} -o output ${LDFLAGS} ${LIBS} $^
hashtest: hash_tests.c hash.c hash.h
	cc -g -o hashtest hash.c hash_tests.c
matops.o: matops.c matops.h
	gcc -c matops.c
clean:
	rm *.o output
