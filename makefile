CC=gcc  # Specify the C compiler to use
CFLAGS=-I.  # List of flags to pass to the compilation command
SDIR = src

default: build

build:
	$(CC) $(SDIR)/main.c -o main
	$(CC) $(SDIR)/P.c -o P -lm
	$(CC) $(SDIR)/S.c -o S
	$(CC) $(SDIR)/L.c -o L
	$(CC) $(SDIR)/G.c -o G

.PHONY: clean

clean:
	rm -f main
	rm -f P
	rm -f S
	rm -f L
	rm -f G

# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
