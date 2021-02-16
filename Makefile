.SUFFIXES: .o .c .h .md .html
.PHONY: all clean install uninstall tutorial

CFLAGS=-O2 -Wall
CC=cc

INSTALL=install
MD=markdown

PROG=qed

INSTALLD=/usr/local
BINDIR=$(INSTALLD)/bin
MANDIR=$(INSTALLD)/share/man/man1

MANPAGE=doc/qed.1
TUTORIAL=doc/qed-tutorial.html

HDRS=\
  vars.h \
  qed.h

MODULES=\
  address \
  blkio \
  com \
  getchar \
  getfile \
  glob \
  main \
  misc \
  move \
  pattern \
  putchar \
  setaddr \
  string \
  subs \
  utf \
  utfio \
  utfstr

OBJS=$(MODULES:=.o)

.c.o:
	$(CC) -c $(CFLAGS) $<

.md.html:
	$(MD) $< > $*.html

all: $(PROG)

tutorial: $(TUTORIAL)

$(PROG): $(OBJS)  $(HDRS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS)

clean:
	rm -f *.o $(PROG) 

install: all $(MANPAGE)
	$(INSTALL) -d $(BINDIR) $(MANDIR)
	$(INSTALL) -s $(PROG) $(BINDIR)/
	$(INSTALL) $(MANPAGE) $(MANDIR)/
	$(INSTALL) $(TUTORIAL) $(MANDIR)/

uninstall:
	rm -f $(BINDIR)/$(PROG)
	rm -f $(MANDIR)/$(MANPAGE)
	rm -f $(MANDIR)/$(TUTORIAL)

