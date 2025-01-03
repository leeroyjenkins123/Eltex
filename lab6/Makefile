CC = gcc
CFLAGS = -Wall -Werror

RUNS = lab6

all: $(RUNS)

lab6: lab6.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(RUNS)

.PHONY: all clean

