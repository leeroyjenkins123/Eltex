CC = gcc
CFLAGS = -Wall -Wextra

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, obj/%.o, $(SRC))
BIN := $(patsubst %.c, bin/%, $(SRC))

.PHONY: all clean

.SECONDARY: $(OBJ)

all: bin $(BIN)

bin:
	mkdir -p bin obj tmp

bin/%: obj/%.o
	$(CC) $< -o $@

obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin obj tmp
