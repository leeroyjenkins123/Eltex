CC = gcc
LDFLAGS = -pthread

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, obj/%.o, $(SRC))
BIN := $(patsubst %.c, bin/%, $(SRC))

.PHONY: all clean

.SECONDARY: $(OBJ)

all: bin $(BIN)

bin:
	mkdir -p bin obj

bin/%: obj/%.o
	$(CC) $< -o $@

obj/%.o: %.c
	$(CC) -c $< -o $@ $(LDFLAGS)

clean:
	rm -rf bin obj
