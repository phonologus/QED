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

%.pdf:  %
	9 troff -man $stem | tr2post | psfonts | ps2pdf - > $target

all:V: a.out
	mv a.out $PROG

a.out:	$OBJS 
	$CC $prereq

clean:V:
	rm -f *.o a.out $PROG *.pdf doc/*.pdf

doc:V:  doc/tutorial.pdf $PROG.1.pdf

install:V: $PROG $QDIR $PROG.1 doc
	mkdir -p $INSTALLD/bin $INSTALLD/man/man1 $INSTALLD/lib $INSTALLD/doc
	cp $PROG $INSTALLD/bin/
	cp $PROG.1 $INSTALLD/man/man1/
	cp $QDIR/* $INSTALLD/lib/
	cp $PROG.1.pdf doc/tutorial.pdf $INSTALLD/doc/

