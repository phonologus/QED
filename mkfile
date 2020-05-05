CFLAGS=-ansi -I. -O -D_POSIX_SOURCE -fno-builtin
CC=gcc

TAR=9 tar
COMPRESS=9 compress

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
  subs \
  utf\
  utfio\
  utfstr

DOCS=\
  doc/qed.1\
  doc/tutorial

ARCHIVE=\
  mkfile\
  $DOCS\
  ${MODULES:%=%.c}\
  $HDRS\
  $QDIR

OBJS=${MODULES:%=%.o}

%.o:  %.c
  $CC -c $CFLAGS $prereq

%.pdf:  %
	9 troff -man $stem | tr2post | psfonts | ps2pdf - > $target

all:V: a.out

a.out:	$OBJS 
	$CC $prereq

clean:V:
	rm -f *.o a.out doc/*.pdf

doc:V:  doc/tutorial.pdf doc/qed.1.pdf

install:V: all doc $QDIR
	mkdir -p $INSTALLD/bin $INSTALLD/man/man1 $INSTALLD/lib $INSTALLD/doc
	cp a.out $INSTALLD/bin/$PROG
	cp doc/qed.1 $INSTALLD/man/man1/
	cp $QDIR/* $INSTALLD/lib/
	cp doc/*.pdf $INSTALLD/doc/

archive:V: $ARCHIVE
	$TAR c $prereq | $COMPRESS > $PROG.tar.Z

unarchive:V: $PROG.tar.Z
	$TAR xzvf $prereq

