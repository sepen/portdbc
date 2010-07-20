DESTDIR =

CC      = gcc

#LIBS   = -L/usr/lib -lcurl -lxml2
#CFLAGS = -I/usr/include -I/usr/include/libxml2

LIBS   += -L/usr/lib $(shell curl-config --libs) $(shell xml2-config --libs)
CFLAGS += -I/usr/include $(shell curl-config --cflags) $(shell xml2-config --cflags)

all: portdbc

portdbc:
	$(CC) -Wall $(LIBS) $(CFLAGS) -o portdbc portdbc.c

clean:
	@rm -f portdbc

install: portdbc
	@install -d $(DESTDIR)/usr/bin
	@install -m 0755 portdbc $(DESTDIR)/usr/bin/portdbc
