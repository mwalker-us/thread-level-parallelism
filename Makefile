# Runs make for all files and zips up grader folder.
all: zip exec_matrix

# Zips a folder with all the files needed for lab1. Not all files may
# be useful to the grader but we include them anyway just in case.
zip: matrix.h matrix.c Makefile
	zip -r lab4.zip matrix.h matrix.c Makefile

# Dependencies and actual program executable work.
exec_matrix: matrix.h matrix.o exec_matrix.o
	gcc matrix.o -O1 -Wall -lpthread -o exec_matrix

exec_matrix.o: matrix.h matrix.c
	gcc -ansi -pedantic -D_POSIX_C_SOURCE=199309L -g -lpthread -c matrix.c

# Cleans the build folder so we can effectively build our program from
# scratch. Useful for cases where their might be a bug in our Makefile.
clean:
	rm -rf *.o exec_matrix lab4.zip