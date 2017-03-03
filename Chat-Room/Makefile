# the shared memory example from OSC 9e Sec. 3.5.1, Fig. 3.17, 3.18

# this must use GNU Make ("make" on Linux and Mac OS X, "gmake" on Solaris)

# compiler options -- C99 with warnings
OPT_GCC = -std=c99 -Wall -Wextra

# compiler options and libraries for Linux, Mac OS X or Solaris
OPT = -D_XOPEN_SOURCE=700
LIB = -lrt

all: cli3 cli9 serv

cli3: client3.c
	gcc $(OPT_GCC) $(OPT) -o cli3 client3.c $(LIB)

cli9: client9.c
	gcc $(OPT_GCC) $(OPT) -o cli9 client9.c $(LIB)

serv: server.c
	gcc $(OPT_GCC) $(OPT) -o serv server.c $(LIB)

run: cli3 cli9 serv
	./serv
	./cli3
	./cli9

clean:
	rm -f cli3 cli9 serv


