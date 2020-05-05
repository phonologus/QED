/*
 * UTFIO
 *
 */

/*
 * Buffered i/o for utf8 <-> unicode.
 *
 */

#include "qed.h"

utfio *
uioinit(int fd, utfio *io)    /* initialise empty buffer */
{
  io->fd=fd;
  io->i=0;
  io->z=UBSIZE;
  return io;
}

utfio *
uioinitrd(int fd, utfio *io)    /* initialise empty read buffer */
{
  io->fd=fd;
  io->i=UBSIZE;
  io->z=UBSIZE;
  return io;
}


/* reading */

int uiofill(utfio* io)
{
  byte *s,*d; 
  int n,i,r,z;

  i=io->i;
  z=io->z;

  if(i<z){          /* shift unused bytes to beginning of buffer */
    i=n=z-i;
    s=&io->b[z];
    d=&io->b[n];
    while(n--)
      *--d=*--s;
  } else i-=z;

  r=read(io->fd,&io->b[i],z-i);
  if(r<0)
    return r;
  io->z=i+r;
  io->i=0;
  return io->z;
}

int
uioread(utfio *io, int *ub, int bsize)
{
  int count;  
  byte *b;
  int z,i,ii;

  count=0;
  i=io->i;
  z=io->z;
  b=&io->b[i];

  while(count<bsize) {
    if(i>=z)          /* buffer is potentially exhausted */
      if(uiofill(io)<0) {  /* try to refill buffer */
        return -1;         /* there was a read error */
      } else {
        if(io->z) {        /* buffer re-filled OK */
          i=io->i;         /* or, source is EOF, but there is a residue */
          z=io->z;
          b=&io->b[i];
        } else {            /* source is EOF, buffer is empty. We're done. */
          *ub++=utfeof;       /* EOF-sentinel in output */
          break;
        }
      }
    ii=convnutf(b,ub,z-i);
    if(ii>0){         /* converted one utf8 sequence OK */
      io->i=(i+=ii);
      b+=ii;
      ub++;
      count++;
    } else if(*ub){  /* not enough bytes to read */
      i+=(*ub);
      *ub=0;
    } else return -1;     /* utf8 decoding error */
  }

  return count;

}

/* writing */

int uioflush(utfio* io)
{
  int i,r;

  i=io->i;

  if((r=write(io->fd,&io->b[0],i))!=i)
    return -1;      /* there was a write error */

  io->z=UBSIZE;
  io->i=0;
  return r;
}

int
uiowrite(utfio *io, int *ub, int bsize)
{
  int count;  
  byte *b;
  int z,i,ii;

  count=0;
  i=io->i;
  z=io->z;
  b=&io->b[i];

  while(count<bsize) {
    if(i>=z)                /* buffer would over-flow */
      if(uioflush(io)<0) {  /* try to empty buffer */
        return -1;          /* there was a write error */
      } else {
        i=io->i;            /* buffer flushed OK. */
        z=io->z;
        b=&io->b[i];
      }
    ii=convnucode(*ub,b,z-i);
    if(ii>0){         /* converted one utf8 sequence OK */
      io->i=(i+=ii);
      b+=ii;
      ub++;
      count++;
    } else if(ii>-5){      /* not enough space in buffer */
      i -= ii;
    } else return -1;     /* utf8 encoding error */
  }

  return count;

}

