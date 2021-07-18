CC=gcc  # Specify the C compiler to use
CFLAGS=-I.  # List of flags to pass to the compilation command
SDIR = src

default: build

build:
	$(CC) $(SDIR)/main.c -o main
	chmod +x main
	$(CC) $(SDIR)/S.c -o S
	chmod +x S
	$(CC) $(SDIR)/P.c -o P -lm
	chmod +x P
	$(CC) $(SDIR)/G.c -o G
	chmod +x G
	$(CC) $(SDIR)/L.c -o L
	chmod +x L

.PHONY: clean

clean:
	rm -f main
	rm -f P
	rm -f S
	rm -f L
	rm -f G

# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# magari alla fine aggiungi folder obj per gli eseguibili
