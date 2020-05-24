.SUFFIXES: .o .c .h
.PHONY: all clean install

CFLAGS=-ansi -O -D_POSIX_SOURCE -fno-builtin -Wall
CC=gcc

PROG=qed

INSTALLD=$(HOME)/opt/$(PROG)

MANPAGE=\
  doc/qed.1

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
	mkdir -p $(INSTALLD)/bin $(INSTALLD)/man/man1
	cp $(PROG) $(INSTALLD)/bin/
	cp $(MANPAGE) $(INSTALLD)/man/man1/

