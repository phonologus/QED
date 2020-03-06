#include "qed.h"

enum {
  BLKBITS = 11,
  BLKSIZE = (1 << (1+BLKBITS)),
  MAXBLOCKS = 4095,
  BLMASK = MAXBLOCKS
};

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

char
*getline(int tl, char *lbuf)
{
	char *bp, *lp;
	int nl;

	extern int read();

	lp = lbuf;
	nl = -((tl<<1) & 0774);
	tl = (tl>>BLKBITS) & BLMASK;
	do {
		if (nl<=0) {
			if (tl==oblock)
				bp = obuff;
			else {
				bp = ibuff;
				if (tl!=iblock) {
					iblock = -1;	/* signal protection */
					blkio(tl, bp, read);
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

int
putline(void)
{
	char *op, *lp;
	int r;
	extern int write();

	modified();
	lp = linebuf;
	r = (oblock<<BLKBITS) + (ooff>>1);	/* ooff may be BLKSIZE! */
	op = obuff + ooff;
	do {
		if (op >= obuff+BLKSIZE) {
			/* delay updating oblock until after blkio succeeds */
			blkio(oblock, op=obuff, write);
			oblock++;
			ooff = 0;
		}
		if((*op = *lp++) == '\n') {
			*op++ = '\0';
			linebp = lp;
			break;
		}
	} while (*op++);
	ooff = (((op-obuff)+3)&~3);
	return (r);
}

void
blkio(int b, char *buf, int (*iofcn)())
{
	if (b>=MAXBLOCKS
	|| (lseek(tfile, ((long) b) * ((long) BLKSIZE), 0)<0L)
	|| (*iofcn)(tfile, buf, BLKSIZE) != BLKSIZE) {
		error('T');
	}
}
