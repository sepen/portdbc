DESTDIR =

PREFIX = /usr/local
DATAROOTDIR = ${prefix}/share
BINDIR = ${exec_prefix}/bin
MANDIR = ${datarootdir}/man

CC = gcc

LIBS = -L/usr/lib -lcurl -L/usr/lib -lxml2
CFLAGS = -Wall -O2 -I/usr/include -I/usr/include/libxml2

all: portdbc

portdbc: defines.h utils.h utils.c portdbc.c
	$(CC) $(CFLAGS) utils.c portdbc.c $(LIBS) -o portdbc

clean:
	@rm -f portdbc

distclean: clean
	@rm -rf Makefile configure config.log config.status autom4te.*

install: portdbc portdbc.8
	@install -d $(DESTDIR)$(BINDIR)
	@install -m 0755 portdbc $(DESTDIR)$(BINDIR)/portdbc
	@install -d $(DESTDIR)$(MANDIR)/man8
	@install -m 0644 portdbc.8 $(DESTDIR)$(MANDIR)/man8/portdbc.8
