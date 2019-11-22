output: main.o
	mpicc -o output main.c
main.o: main.c
	mpicc -c main.c
clean:
	rm *.o output

