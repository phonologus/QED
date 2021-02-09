.SUFFIXES: .o .c .h
.PHONY: all clean install uninstall

CFLAGS=-O2 -Wall
CC=cc

INSTALL=install

PROG=qed

INSTALLD=/usr/local
BINDIR=$(INSTALLD)/bin
MANDIR=$(INSTALLD)/share/man/man1

MANPAGE=doc/qed.1
TUTORIAL=doc/qed-tutorial.1

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

all: $(PROG)

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

