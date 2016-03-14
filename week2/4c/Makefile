CFILES = $(wildcard *.c)
TARGETS = $(CFILES:.c=.out)
CC ?= gcc
CFLAGS = -Wall -g3

all: $(TARGETS)

$(TARGETS): $(CFILES)
	$(CC) $(CFLAGS) -o $@ $(@:.out=.c)

clean:
	rm -f $(TARGETS)
