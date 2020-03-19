#include "qed.h"

/*
 * addr_t type:
 *
 * The offset into the tempfile is stored as a cookie, containing
 * the block number, the offset, and a mark bit, in a packed
 * representation.
 *
 * Offsets are multiples of 4, so have two spare low-order bits.
 * The LSB is used as the mark bit
 *
 *                        3 2 1 0
 *                        4 3 2 M
 *                        ----|
 * significant bits of offset | mark bit
 */

enum {
  ALIGN = 2,      /* align strings to (2^ALIGN)-byte boundaries in a block */
  ALIGNMASK = ((1 << ALIGN) - 1),
  SHUNT = (ALIGN - 1),    /* mark bit requires 1 bit, the rest are redundant */
  BLKBITS = 12,
  BLKSHIFT = BLKBITS-SHUNT,
  BLKSIZE = (1 << BLKBITS),
  OFFMASK = BLKSIZE - ALIGNMASK,
  LOTSOFBITS = 12,
  MAXBLOCKS = ((1 << LOTSOFBITS)-1),
  BLMASK = MAXBLOCKS
};

enum {
  RD=0,
  WR=1
};

#define getblock(a) (((a)>>BLKSHIFT) & BLMASK)
#define getoffset(a) (((a)<<SHUNT) & OFFMASK)
#define cookie(b,o) (((b)<<BLKSHIFT) + ((o)>>SHUNT))
#define align(a) (((a)+ALIGNMASK)&~ALIGNMASK)

char	ibuff[BLKSIZE];
int	iblock = -1;
int	oblock = 0;
char	obuff[BLKSIZE];
int	ooff;		/* offset of next byte in obuff */

void
initio(void)
{
	iblock = -1;
	oblock = 0;
	ooff = 0;
}

char *
getline(addr_t tl, char *lbuf)
{
	char *bp, *lp;
	int nl;

	lp = lbuf;
        nl = -getoffset(tl);
        tl = getblock(tl);
	do {
		if (nl<=0) {
			if (tl==oblock)
				bp = obuff;
			else {
				bp = ibuff;
				if (tl!=iblock) {
					blkio(tl, bp, RD);
					iblock = tl;
				}
			}
			tl++;
			bp -= nl;
			nl += BLKSIZE;
		}
		nl--;
	} while (*lp++ = *bp++);
	return(lbuf);
}

addr_t
putline(void)
{
	char *op, *lp;
	addr_t r;

	modified();
	lp = linebuf;
	r = cookie(oblock,ooff);	/* ooff may be BLKSIZE! */
	op = obuff + ooff;
	do {
		if (op >= obuff+BLKSIZE) {
			/* delay updating oblock until after blkio succeeds */
			blkio(oblock, op=obuff, WR);
			oblock++;
			ooff = 0;
		}
		if((*op = *lp++) == '\n') {
			*op++ = '\0';
			linebp = lp;
			break;
		}
	} while (*op++);
	ooff = align(op-obuff);
	return (r);
}

void
blkio(int b, char *buf, int rw)
{
	if (b>=MAXBLOCKS)
		error('T');
	if(lseek(tfile, ((long) b) * ((long) BLKSIZE), 0)<0L)
		error('T');
	switch(rw) {
		case RD:
			if(read(tfile, buf, BLKSIZE) != BLKSIZE) 
				error('T');
			break;
		case WR:
			if(write(tfile, buf, BLKSIZE) != BLKSIZE) 
				error('T');
			break;
		default:
			error('T');
	}
}
