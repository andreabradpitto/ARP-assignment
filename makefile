# 2019 ARP assignment V2.0
# Andrea Pitto - S3942710
# 10 - 07 - 2020

# Specify the C compiler to use
CC=gcc
# List of flags to pass to the compilation command
CFLAGS=-I.
SDIR = src

default: build

build:
	$(CC) $(SDIR)/main.c -o main && chmod +x main
	$(CC) $(SDIR)/S.c -o S && chmod +x S
	$(CC) $(SDIR)/P.c -o P -lm && chmod +x P
	$(CC) $(SDIR)/G.c -o G && chmod +x G
	$(CC) $(SDIR)/L.c -o L && chmod +x L

.PHONY: clean

clean:
	rm -f main
	rm -f P
	rm -f S
	rm -f L
	rm -f G
	rm -f log.txt
