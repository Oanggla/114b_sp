CC = gcc
CFLAGS = -Wall -pthread

TARGETS = bank producer_consumer philosopher

all: $(TARGETS)

bank: bank.c
	$(CC) $(CFLAGS) bank.c -o bank

producer_consumer: producer_consumer.c
	$(CC) $(CFLAGS) producer_consumer.c -o producer_consumer

philosopher: philosopher.c
	$(CC) $(CFLAGS) philosopher.c -o philosopher

run_bank: bank
	./bank

run_producer_consumer: producer_consumer
	./producer_consumer

run_philosopher: philosopher
	./philosopher

clean:
	rm -f $(TARGETS) *.exe *.o
