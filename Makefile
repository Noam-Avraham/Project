# Makefile for building the C module and other components

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Sources and objects for the C module
CSRC = symnmf.c
COBJ = $(CSRC:.c=.o)

# Default target
all: module

# Build the C module
module: $(COBJ)
	$(CC) $(CFLAGS) -shared -o libsymnmf.so $(COBJ)

# Pattern rule for compiling C sources
%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(COBJ) libsymnmf.so

.PHONY: all module clean
