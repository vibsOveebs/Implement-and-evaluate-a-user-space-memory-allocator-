CC=gcc -g -std=gnu99 -Wall -Wextra -Wno-unused-parameter

default:
	$(CC) -c psumemory.c -fPIC -o psulibmem.o
	$(CC) -shared psulibmem.o -lm -o psulibmem.so
	$(CC) $(FILE) psulibmem.so -o prog3

clean:
	rm *.o
	rm *.so