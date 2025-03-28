CC = gcc
CFLAGS = -Wall -Werror

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, obj/%.o, $(SRC))
BIN := $(patsubst %.c, bin/%, $(SRC))

.PHONY: all clean

all: bin $(BIN)

bin:
	mkdir -p bin obj

bin/%: obj/%.o
	$(CC) $< -o $@

obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin obj

