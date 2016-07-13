TARGET = flap
SDIR = src
PREFIX = /usr/bin

CC = gcc
CFLAGS = -Wall
LIBS = -lxcb -lxcb-ewmh -lxcb-icccm -lxcb-randr

flap: $(SDIR)/flap.c
	$(CC) $(CFLAGS) $(SDIR)/flap.c $(LIBS) -o $(TARGET)

install:
	cp -f flap /usr/bin/flap

uninstall:
	rm -f /usr/bin/flap
