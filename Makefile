CC=gcc
CFLAGS=-Wall -Wextra
SRC=main.c interactions.c
BIN=malaria

all:
	$(CC) $(SRC) $(CFLAGS) -o $(BIN)

debug:
	$(CC) $(SRC) $(CFLAGS) -o $(BIN) -ggdb

clean:
	rm -f $(BIN)
