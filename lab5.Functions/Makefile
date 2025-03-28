CC = gcc
CFLAGS = -Wall -Werror
LAB5FLAG = -fno-stack-protector -no-pie

RUNS = switching_to_the_desired_return_address

all: $(RUNS)

switching_to_the_desired_return_address: switching_to_the_desired_return_address.c
	$(CC) -std=c99 $(LAB5FLAG) -g -o $@ $<

clean:
	rm -f $(RUNS)

.PHONY: all clean
