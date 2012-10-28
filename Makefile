APPS    =glex
PREFIX =/usr

CFLAGS  =`pkg-config --cflags gtk+-2.0`
LDFLAGS =`pkg-config --libs gtk+-2.0 gthread-2.0`

CFLAGS  +=-I/usr/include/libxml2
LDFLAGS +=-lxml2

$(APPS):
	cc $(CFLAGS) -o $(APPS) $(APPS).c $(LDFLAGS)

all: $(APPS) 

install: 
	if [ ! -d $(DESTDIR)$(PREFIX)/bin/ ]; then mkdir -p $(DESTDIR)$(PREFIX)/bin/; fi
	cp $(APPS) $(DESTDIR)$(PREFIX)/bin/$(APPS)

uninstall: 
	 rm $(DESTDIR)$(PREFIX)/bin/$(APPS)

clean:
	rm -f $(APPS)
