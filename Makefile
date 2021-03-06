CC=gcc
CFLAGS=-Wall -Wextra -O3
SRC=main.c interactions.c conveyor.c
BIN=malaria

all:
	$(CC) $(SRC) $(CFLAGS) -o $(BIN)

debug:
	$(CC) $(SRC) $(CFLAGS) -o $(BIN) -ggdb

clean:
	rm -f $(BIN)
