CC = gcc
CFLAGS = -Wall -Werror

RUNS = lab_3_1 lab_3_2 lab_3_3 lab_3_4

all: $(RUNS)

lab_3_1: lab_3_1.c
	$(CC) $(CFLAGS) -o $@ $<

lab_3_2: lab_3_2.c
	$(CC) $(CFLAGS) -o $@ $<

lab_3_3: lab_3_3.c
	$(CC) $(CFLAGS) -o $@ $<

lab_3_4: lab_3_4.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(RUNS)

.PHONY: all clean

