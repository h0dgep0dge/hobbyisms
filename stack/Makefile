all: math

test: math
	./math

math: math.c stacklib.o stacklib.h
	gcc -o math math.c stacklib.o

lib.o: stacklib.h stacklib.c
	gcc -c stacklib.c

clean:
	rm -f *.o math
