
CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors


all: symnmf

symnmf: symnmf.o
	$(CC) $(CFLAGS) symnmf.o -o symnmf -lm

symnmf.o: symnmf.c symnmf.h structure.h
	$(CC) $(CFLAGS) -c symnmf.c


