CFLAGS=-ansi -I. -O -D_POSIX_SOURCE -fno-builtin
CC=gcc

PROG=qed

INSTALLD=$HOME/opt/$PROG

QDIR=q

HDRS=\
  vars.h \
  qed.h

MODULES=\
  address\
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
  subs 

OBJS=${MODULES:%=%.o}

%.o:  %.c
  $CC -c $CFLAGS $prereq

all:V: a.out
	mv a.out $PROG

a.out:	$OBJS 
	$CC $prereq

debug:  $OBJS debug.o
	$CC $prereq

clean:V:
	rm -f *.o a.out $PROG

install:V: $PROG $PROG.1 $QDIR
	mkdir -p $INSTALLD/bin $INSTALLD/man/man1 $INSTALLD/lib
	cp $PROG $INSTALLD/bin/
	cp $PROG.1 $INSTALLD/man/man1/
	cp $QDIR/* $INSTALLD/lib/

