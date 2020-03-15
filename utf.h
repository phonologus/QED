/*
 * UTF
 */

#ifndef __UTF__
#define __UTF__

enum {
  utfbytes=4
};

#define convutf(p,z) convnutf(p,z,utfbytes)
#define convucode(p,z) convnucode(p,z,utfbytes)

int convnutf(unsigned char *, int *, int);
int convnucode(unsigned int,unsigned char *, int);

#endif

