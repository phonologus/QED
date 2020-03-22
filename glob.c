#include "qed.h"

enum {
  ALWAYS = 2,
  GBSIZE = 256
};

void
until(int nfl, int n)
{
	int c, type;
	int ubuf[GBSIZE];

	c = getchar();
	type=posn(c, utfstr_ft_);
	if(type < 0){
		if(nfl==0)
			error('x');
		ungetchar(c);
	}
	getglob(ubuf);
	settruth(FALSE);
	if(nfl && n<=0)
		return;	/* do it zero times */
	do ; while (exglob(ubuf, ubuf) &&
		   !((nfl && --n==0) || 
		     (type != ALWAYS && type == truth())));
}

void
global(int k)
{
	addr_i a1;
	int globuf[GBSIZE];
	struct buffer *startbuf;

	if(gflag++)
		error('g');
	setall();
	if(nextchar() == '\n')
		error('x');
	compile(getchar());
	getglob(globuf);
	for (a1=zero; ++a1<=dol;) {
		core[a1] &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(a1)==k)
			core[a1] |= 01;
	}
	startbuf = curbuf;
	for (a1=zero; ++a1<=dol; ) {
		if (core[a1] & 01) {
			core[a1] &= ~01;
			dot = a1;
			if (!exglob(globuf, utfstr_p))
				break;
			chngbuf(startbuf-buffer);
			a1 = zero;
		}
	}
	chngbuf(startbuf-buffer);
	gflag=FALSE;
}

void
globuf(int k)
{
	struct buffer *bp;
	int globbuf[GBSIZE];

	if (biggflag++)
		error('G');
	if (nextchar() =='\n')
		error('x');
	compile(getchar());
	getglob(globbuf);
	for (bp=buffer; bp < &buffer[NBUFS]; bp++)
		bp->gmark = FALSE;
	ncom('G');	/* mark interesting buffers */
	for (bp=buffer; bp < &buffer[NBUFS]; bp++)
		if (bp->gmark == k) {
			chngbuf(bp-buffer);
			if (!exglob(globbuf, utfstr_fnl))
				break;
		}
	biggflag = FALSE;
}

void
getglob(int globuf[])
{
	int *gp;
	int c;
	int getchar();
	gp = globuf;
	while ((c = getquote(utfstr_nl, getchar)) != '\n') {
		if (c==EOF)
			error('x');
		*gp++ = unescape(c);
		if (gp >= &globuf[GBSIZE-2])
			error('l');
	}
	*gp++ = '\n';
	*gp++ = 0;
}

int
exglob(int *cmd, int *dflt)
{
        union pint_t uc;
	int nesting;

	uc.p=eqstr(cmd,utfstr_nl)? dflt : cmd;
	pushinp(GLOB, uc, FALSE);
	nesting = ++nestlevel;
	commands();
	if (nesting!=nestlevel)
		return(FALSE);
	nestlevel--;
	return(TRUE);
}
