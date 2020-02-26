CFLAGS=-ansi -I. -O -D_POSIX_SOURCE
CC=gcc

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

%.o:  %.c $HDRS
  $CC -c $CFLAGS $prereq

all:V: a.out

a.out:	$OBJS 
	$CC $prereq

clean:V:
	rm -f *.o a.out

