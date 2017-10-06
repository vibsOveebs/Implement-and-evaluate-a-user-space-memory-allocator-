To build: make FILE=<filename.c>

Where <filename.c> is the C program to be linked against the psulibmem. A shared library, psulibmem.so, will be generated and the entered C file will be linked against it.

To clean up object files and shared library: make clean