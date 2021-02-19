.SUFFIXES: .o .c .h
.PHONY: all clean install uninstall

CFLAGS=-O2 -Wall
CC=cc

INSTALL=install

PROG=qed

INSTALLD=/usr/local
BINDIR=$(INSTALLD)/bin
MANDIR=$(INSTALLD)/share/man/man

MSECTION=1
TSECTION=7

MANPAGE=doc/qed.$(MSECTION)
TUTORIAL=doc/qed-tutorial.$(TSECTION)

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

$(PROG): $(OBJS)  $(HDRS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS)

clean:
	rm -f *.o $(PROG) 

install: all $(MANPAGE) $(TUTORIAL)
	$(INSTALL) -d $(BINDIR) $(MANDIR)$(MSECTION)
	$(INSTALL) -d $(BINDIR) $(MANDIR)$(TSECTION)
	$(INSTALL) -s $(PROG) $(BINDIR)/
	$(INSTALL) $(MANPAGE) $(MANDIR)$(MSECTION)/
	$(INSTALL) $(TUTORIAL) $(MANDIR)$(TSECTION)/

uninstall:
	rm -f $(BINDIR)/$(PROG)
	rm -f $(MANDIR)$(MSECTION)/$(MANPAGE)
	rm -f $(MANDIR)$(TSECTION)/$(TUTORIAL)

