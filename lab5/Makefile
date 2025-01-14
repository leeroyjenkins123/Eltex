CC = gcc
CFLAGS = -Wall -Werror
LAB5FLAG = -fno-stack-protector -no-pie

RUNS = lab5

all: $(RUNS)

lab5: lab5.c
	$(CC) -std=c99 $(LAB5FLAG) -g -o $@ $<

clean:
	rm -f $(RUNS)

.PHONY: all clean

