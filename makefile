CC=gcc  # Specify the C compiler to use
CFLAGS=-I.  # List of flags to pass to the compilation command
SDIR = src

default: build

build:
	$(CC) $(SDIR)/main.c -o main
	$(CC) $(SDIR)/S.c -o S
	$(CC) $(SDIR)/P.c -o P -lm
	$(CC) $(SDIR)/G.c -o G
	$(CC) $(SDIR)/L.c -o L

.PHONY: clean

clean:
	rm -f main
	rm -f P
	rm -f S
	rm -f L
	rm -f G

# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# magari alla fine aggiungi folder obj per gli eseguibili
