CFLAGS=-g
CC=mpicc
OBJS=main.o hash.o
output: ${OBJS}
	mpicc ${CFLAGS} -o output ${LDFLAGS} ${LIBS} $^
clean:
	rm *.o output

