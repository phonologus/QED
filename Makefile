.SUFFIXES: .o .c .h
.PHONY: all clean

CFLAGS=-ansi -O -D_POSIX_SOURCE -fno-builtin -Wall
CC=gcc

PROG=qed

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

