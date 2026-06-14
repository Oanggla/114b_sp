CC = gcc
CFLAGS = -Wall -g

TARGETS = fork_exec file_io redirect

.PHONY: all clean

all: $(TARGETS)

fork_exec: fork_exec.c
	$(CC) $(CFLAGS) -o fork_exec fork_exec.c

file_io: file_io.c
	$(CC) $(CFLAGS) -o file_io file_io.c

redirect: redirect.c
	$(CC) $(CFLAGS) -o redirect redirect.c

clean:
	rm -f $(TARGETS) *.o *.txt *.exe
