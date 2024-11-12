CC = gcc
CFLAGS = -Wall -Werror

RUNS = lab_2_1 lab_2_2 lab_2_3 lab_2_4

TASK1 = lab_2_1.c
TASK2 = lab_2_2.c
TASK3 = lab_2_3.c
TASK4 = lab_2_4.c


all: $(RUNS)

lab_2_1: $(TASK1)
	$(CC) $(CFLAGS) -o $@ $<

lab_2_2: $(TASK2)
	$(CC) $(CFLAGS) -o $@ $<

lab_2_3: $(TASK3)
	$(CC) $(CFLAGS) -o $@ $<

lab_2_4: $(TASK4)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(RUNS)

.PHONY: all clean

