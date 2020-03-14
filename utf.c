/*
 * UTF
 */

#include "utf.h"

/*
 * convnutf() returns a pointer to the next byte after a valid utf
 * encoding at p, or (char*)0 if p does not point to a valid utf
 * encoding. It additionally puts the decoded value into
 * the integer reference z, or 0 if utf is not valid.
 *
 * If the sequence exceeds n bytes, and hasn't yet validated,
 * (char*)0 is returned, and z is set to the number
 * of missing bytes.
 *
 * convnucode() is the inverse operation, taking a unicode
 * codepoint c, and storing the utf8 representation in (at most)
 * n bytes of p.
 *
 * The number of bytes used is the return value. If the sequence
 * does not fit in the buffer, a negative value is returned, representing
 * the number of bytes short of a valid sequence the buffer is.
 * 
 */

unsigned char *
convnutf(unsigned char *p, int *z, int n)
{
  int c,u,i;

  i=1;

  if(0>--n)
    goto prem;
  c=*p++;
  u=c;
  --i;

  if (0==(c & 0200))
    goto valid;             /* 0bbbbbbb : valid ascii */

  u&=~0200;
  c <<= 1; 
  if (0==(c & 0200))
    goto invalid;      /* 10bbbbbb : error (aux byte in worng place) */

  u&=~0100;
  c <<= 1; 
  i++;
  if (0==(c & 0200))
    goto two;             /* 110bbbbb : start of two-byte sequence */

  u&=~040;
  c <<= 1; 
  i++;
  if (0==(c & 0200))
    goto three;           /* 1110bbbb : start of three-byte sequence */

  u&=~020;
  c <<= 1; 
  i++;
  if (0==(c & 0200))
    goto four;            /* 11110bbb : start of four-byte sequence */

  goto invalid;        /* 11111bbb : invalid range (unless we want five..) */

four:
  if(0>--n)
    goto prem;
  u <<= 6;
  c=*p++;
  --i;
  if (0==(c & 0200))
    goto invalid;      /* 0bbbbbbb : ascii character in wrong place */
  u|=(c&~0200);
  c <<= 1; 
  if (0==(c & 0200))
    goto three;           /* 10bbbbbb : aux byte in expected place */

  goto invalid;        /* 11bbbbbb : start byte in wrong place */

three:
  if(0>--n)
    goto prem;
  u <<= 6;
  c=*p++;
  --i;
  if (0==(c & 0200))
    goto invalid;      /* 0bbbbbbb : ascii character in wrong place */
  u|=(c&~0200);
  c <<= 1; 
  if (0==(c & 0200))
    goto two;             /* 10bbbbbb : aux byte in expected place */

  return 0;               /* 11bbbbbb : start byte in wrong place */

two:
  if(0>--n)
    goto prem;
  u <<= 6;
  c=*p++;
  --i;
  if (0==(c & 0200))
    goto invalid;      /* 0bbbbbbb : ascii character in wrong place */
  u|=(c&~0200);
  c <<= 1; 
  if (0==(c & 0200))
    goto valid;             /* 10bbbbbb : final aux byte */

  goto invalid;        /* 11bbbbbb : start byte in wrong place */

prem:
  *z=i;
  return (char*)0;

invalid:
  *z=0;
  return (char*)0;

valid:
  *z=u;
  return p;

}

int
convnucode(unsigned int c, unsigned char *p, int n)
{
  *p='\0';

  if(c<(1<<8))
    goto ascii;  
  if(c<(1<<12))
    goto twobytes;
  if(c<(1<<16))
    goto threebytes;
  if(c<(1<<21))
    goto fourbytes;

  /* too big */

  return -5;

ascii:
  if(n<1)
    return n-1;
  p+=1;
  *p--='\0';
  *p=(unsigned char)c;
  return 1;

twobytes:
  if(n<2)
    return n-2;
  p+=2;
  *p--='\0';
  *p--=(unsigned char)((0200)|(c&077));
  c>>=6;
  *p=(unsigned char)((0300)|(c&077));
  return 2;

threebytes:
  if(n<3)
    return n-3;
  p+=3;
  *p--='\0';
  *p--=(unsigned char)((0200)|(c&077));
  c>>=6;
  *p--=(unsigned char)((0200)|(c&077));
  c>>=6;
  *p=(unsigned char)((0340)|(c&077));
  return 3;

fourbytes:
  if(n<4)
    return n-4;
  p+=4;
  *p--='\0';
  *p--=(unsigned char)((0200)|(c&077));
  c>>=6;
  *p--=(unsigned char)((0200)|(c&077));
  c>>=6;
  *p--=(unsigned char)((0200)|(c&077));
  c>>=6;
  *p=(unsigned char)((0360)|(c&077));
  return 4;
  
}
