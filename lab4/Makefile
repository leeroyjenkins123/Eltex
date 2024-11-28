CC = gcc
CFLAGS = -Wall -Werror

RUNS = lab4

all: $(RUNS)

lab4: lab4.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(RUNS)

.PHONY: all clean

