TARGET = flap
SDIR = src
PREFIX = /usr/bin
BINDIR = ${PREFIX}/bin

CC = gcc
CFLAGS = -Wall
LIBS = -lxcb -lxcb-ewmh -lxcb-icccm -lxcb-randr

flap: $(SDIR)/flap.c
	$(CC) $(CFLAGS) $(SDIR)/flap.c $(LIBS) -o $(TARGET)

install:
	install -D -m 755 flap ${DESTDIR}${BINDIR}/flap

uninstall:
	rm -f /usr/bin/flap
