CC = gcc
CFLAGS = -Wall -Werror

SRC = task_1_1.c task_1_2.c task_1_3.c task_1_4.c
RUNS = task_1_1 task_1_2 task_1_3 task_1_4

all: $(RUNS)

task_1_1: task_1_1.c
	$(CC) $(CFLAGS) -o $@ $<

task_1_2: task_1_2.c
	$(CC) $(CFLAGS) -o $@ $<

task_1_3: task_1_3.c
	$(CC) $(CFLAGS) -o $@ $<

task_1_4: task_1_4.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(RUNS)

.PHONY: all clean

