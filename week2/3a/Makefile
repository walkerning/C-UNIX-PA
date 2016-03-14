CFILES = $(wildcard *.c)
OUT = test.out
OBJS = $(CFILES:.c=.o)
CC ?= gcc
CFLAGS = -Wall -DDEBUG -g3

all: $(OUT)

test.out: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)
