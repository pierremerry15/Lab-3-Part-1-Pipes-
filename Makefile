CC=gcc
CFLAGS=-Wall -Wextra -O2

all: pipes_proc2 pipes_processes3

pipes_proc2: pipes_processes2.c
	$(CC) $(CFLAGS) $< -o $@

pipes_processes3: pipes_processes3.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f pipes_proc2 pipes_processes3
