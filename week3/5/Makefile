CFILES = $(wildcard *.c)
OUT = freport
OBJS = $(CFILES:.c=.o)
CC ?= gcc
CFLAGS = -Wall -DDEBUG -g3

all: $(OUT)

freport: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)
