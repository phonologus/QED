/*
 * UTF
 */

#ifndef __UTF__
#define __UTF__

enum {
  utfbytes=4,
  utfeof=-1
};

typedef unsigned char byte;

#define convutf(p,z) convnutf(p,z,utfbytes)
#define convucode(p,z) convnucode(p,z,utfbytes)

int convnutf(byte *, int *, int);
int convnucode(int, byte *, int);

#endif

