.SUFFIXES: .o .c .h
.PHONY: all clean install

CFLAGS=-ansi -O -D_POSIX_SOURCE -fno-builtin -Wall
CC=gcc

PROG=qed

INSTALLD=$(HOME)/opt/$(PROG)
BINDIR=$(INSTALLD)/bin
MANDIR=$(INSTALLD)/man/man1

MANPAGE=\
  doc/qed.1 \
  doc/qed-tutorial.1

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
	$(CC) -o $(PROG) $(OBJS)

clean:
	rm -f *.o $(PROG) 

install: all $(MANPAGE)
	rm -rf $(INSTALLD)
	mkdir -p $(BINDIR) $(MANDIR)
	cp $(PROG) $(BINDIR)/
	cp $(MANPAGE) $(MANDIR)/

