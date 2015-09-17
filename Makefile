CC=gcc
CFLAGS=-Wall -Wextra -O0
SRC=main.c interactions.c
BIN=malaria
CONVEYOR=conveyor.c
CONVEYOR_DISCRETE=conveyor_discrete.c

all:
	$(CC) $(SRC) $(CONVEYOR) $(CFLAGS) -o $(BIN)

conveyor_discrete:
	$(CC) $(SRC) $(CONVEYOR_DISCRETE) $(CFLAGS) -o $(BIN) -lm

debug:
	$(CC) $(SRC) $(CONVEYOR) $(CFLAGS) -o $(BIN) -ggdb

clean:
	rm -f $(BIN)
