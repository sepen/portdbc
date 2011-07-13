CURL_CONFIG = /usr/bin/curl-config
CURL_LIBS   = $(shell $(CURL_CONFIG) --libs)
CURL_CFLAGS = $(shell $(CURL_CONFIG) --cflags)

XML2_CONFIG = /usr/bin/xml2-config
XML2_LIBS   = $(shell $(XML2_CONFIG) --libs)
XML2_CFLAGS = $(shell $(XML2_CONFIG) --cflags)

DESTDIR =

BINDIR  = /usr/bin
MANDIR  = /usr/man

CC      = gcc

LIBS   += -L/usr/lib $(CURL_LIBS) $(XML2_LIBS)
CFLAGS += -I/usr/include $(CURL_CFLAGS) $(XML2_CFLAGS)

all: portdbc

portdbc: portdbc.c
	$(CC) -Wall $(LIBS) $(CFLAGS) -o portdbc portdbc.c

clean:
	@rm -f portdbc

install: portdbc portdbc.8
	@install -d $(DESTDIR)$(BINDIR)
	@install -m 0755 portdbc $(DESTDIR)$(BINDIR)/portdbc
	@install -d $(DESTDIR)$(MANDIR)/man8
	@install -m 0644 portdbc.8 $(DESTDIR)$(MANDIR)/man8/portdbc.8
