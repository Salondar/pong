CC= gcc
SRC= pong.c
PROG= pong
CFLAGS= -g -Wall -pedantic
LDFLAGS= -lraylib -lm

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(PROG)