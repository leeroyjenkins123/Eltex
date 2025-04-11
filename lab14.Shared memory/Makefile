CC = gcc
CFLAGS = -Wall -Wextra

SRC := $(wildcard *.c)
POSIX_SRC := $(filter posix_%.c, $(SRC))
SYSV_SRC := $(filter-out posix_%.c, $(SRC))

OBJ_SYSV := $(patsubst %.c, obj/sysv/%.o, $(SYSV_SRC))
OBJ_POSIX := $(patsubst %.c, obj/posix/%.o, $(POSIX_SRC))

BIN_SYSV := $(patsubst %.c, bin/sysv/%, $(SYSV_SRC))
BIN_POSIX := $(patsubst %.c, bin/posix/%, $(POSIX_SRC))

.PHONY: all clean

.SECONDARY: $(OBJ_SYSV) $(OBJ_POSIX)

all: bin $(BIN_SYSV) $(BIN_POSIX)

bin:
	mkdir -p bin/sysv bin/posix obj/sysv obj/posix

bin/sysv/%: obj/sysv/%.o
	$(CC) $< -o $@ -lncurses -pthread 

bin/posix/%: obj/posix/%.o
	$(CC) $< -o $@ -lrt

obj/sysv/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

obj/posix/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lrt

clean:
	rm -rf bin obj 
	rm sysv_shared_memory posix_shader_memory
