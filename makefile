CC=gcc -g -std=gnu99 -Wall -Wextra -Wno-unused-parameter

FILE=mixed_workload.c
default:
	$(CC) -c psumemory.c -fPIC -o psulibmem.o -lrt
	$(CC) -shared psulibmem.o -lm -o psulibmem.so
	$(CC) $(FILE) psulibmem.so -o prog3 -lrt

clean:
	rm *.o
	rm *.so

