CC = gcc
CFLAGS = -Wall -Werror

RUNS = lab_1_1 lab_1_2 lab_1_3 lab_1_4

all: $(RUNS)

lab_1_1: lab_1_1.c
	$(CC) $(CFLAGS) -o $@ $<

lab_1_2: lab_1_2.c
	$(CC) $(CFLAGS) -o $@ $<

lab_1_3: lab_1_3.c
	$(CC) $(CFLAGS) -o $@ $<

lab_1_4: lab_1_4.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(RUNS)

.PHONY: all clean

