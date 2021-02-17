#include "qed.h"

/* getnum() & qatoi() work even on non-ASCII systems */

int digits[] = {'0','1','2','3','4','5','6','7','8','9','\0'};

int
gethexn(int n)
{
	int d, i;
	d=0;
	while((n-->0)&&((i=posn(nextchar(), hex)) >= 0)){
		getchar();
		d = d*16 + i;
	}
	if(n>0)
		return -1;	/* error: not enough digits */
	return d;
}

int
getucs(int n)
{
	int c;
	if((c=gethexn(n)) < 0)
		error('U');	/* error: malformed \[uU] special */
	if(c>0x10ffff)
		error('U');	/* error: out of range (RFC 3629) */
	if(c<0xd800 || c>0xdfff)
		return c;	/* yay! valid UCS code point */
	error('U');		/* error: out of range (RFC 3629) */
	/* not reached */
	return -1;
}

int
getnum(void)
{
	int n, i;
	n=0;
	while((i=posn(nextchar(), digits)) >= 0){
		getchar();
		n = n*10 + i;
	}
	return(n);
}

int
getsigned(void)
{
	int sign;
	if(nextchar()=='-'){
		getchar();
		sign = -1;
	}
	else
		sign = 1;
	return(getnum()*sign);
}

int
qatoi(int *s)
{
	int n, i;
	int sign;

	n = 0;
	sign = 1;
	if(*s == '-'){
		sign = -1;
		s++;
	}
	while(*s){
		i = posn(*s++, digits);
		if(i < 0)
			return(sign*n);	/* error handling done elsewhere */
		n = n*10 + i;
	}
	return(sign*n);
}

int
alldigs(int *s)
{
	if(*s == '-')
		s++;
	while(*s)
		if(posn(*s++, digits) < 0)
			return(FALSE);
	return(TRUE);
}

int bname[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m',
               'n','o','p','q','r','s','t','u','v','w','x','y','z',
               'A','B','C','D','E','F','G','H','I','J','K','L','M',
               'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
               '{','|','}','~','\0'};

int
getname(int e)
{
	return(getnm(e, getc));
}

int
getaz(int e)
{
	return(getnm(e, getchar));
}

int
getnm(int e, int (*f)())
{
	int i;
	i = posn((*f)(), bname);
	if(i < 0)
		error(e);
	return(i);
}

int	special[] = {'x','g','B','b','B','c', 'f','F',
                     'l','p','r','z','N','"','\\','\'','x','u','U','\0'};
int	qcount;
int	afterq;

int
getchar(void)
{
	int c;
	int peek;

	peek=peekc;
	c=getc();
	if(tflag && !peek && stackp!= &stack[0]){
		if(c==EOF)
			putstr(utfstr_starEOFstar);
		else
			putchr(c);
	}
	return(c);
}

int
getc(void)
{
        union pint_t uc; 
	int c;
	struct buffer *bufp;
	struct stack *sp;
	addr_i lp;
	int numeric;
	int delta;
	int literal;
	int i;
	int *p;
	static int bufbuf[LBSIZE];

	if((c = peekc))	/* assignment = */
		peekc = 0;
	else if(qcount) {
		--qcount;
		c = '\\';
	} else if((c = afterq))	/* assignment = */
		afterq = 0;
	else {
		for(;;popinp()){
			sp=stackp;
			switch(sp->type){
			case TTY:
			case XTTY:
				if(cprflag && sp == &stack[0] &&
					lastttyc == '\n')
					write(2, "*", 1);
				c = ttyc();
				if(sp != &stack[0] && c == '\n') {
					if(stackp->type == TTY)
						continue;
					else		/* XTTY */
						popinp();
				}
				break;
			case GLOB:
				if ((c = *stackp->globp++) == 0){
					popinp();
					c = EOF;
				}
				break;
			case STRING:
			case BFILEN:
			case PAT:
			case RHS:
			case BRWS:
			case FILEN:
				if(string[sp->strname].len < sp->charno){
					c=EOF;
					continue;
				}
				c=string[sp->strname].str[sp->charno++];
				if(c==0)
					continue;
				break;
			case BUF:
				bufp=sp->bufptr;
				if(bufp==curbuf)
					syncbuf();
				lp=bufp->zero+sp->lineno;
				if(bbempty){
					if(lp>bufp->dol)
						continue;
					p = getline(core[lp], bufbuf);
					bbempty = FALSE;
					for(i=sp->charno; i-- && *p; p++)
						;
				}else
					p = bufbuf+sp->charno;
				sp->charno++;
				if((c = *p) == '\0'){
					/*
					 * Leave the last newline in the buffer out.
					 */
					bbempty = TRUE;
					if(lp>=bufp->dol)
						continue;
					sp->lineno++;
					sp->charno=0;
					c='\n';
				}
			}
			break;
		}
		literal = FALSE;
		if(c == '\\' && !stackp->literal){
		Getc:
			c = getc();
			i = posn(c, cspec);
			switch(i) {
			case LITERAL:
				if(posn(peekc=getc(), utfstr_bfFlprz) >= 0)
					literal = TRUE;
				goto Getc;	/* business as usual... */
			case QUOTE:
				c = '\\';
				break;
			case NEWL:
				c = '\n';
				break;
			case STRING:
				delta = 0;
				numeric = 0;
				c = getc();
				if(c == '#'){
					numeric++;
					c = getc();
				}
				if(c == '+' || c == '-'){
					delta = 1;
					if(c == '-')
						delta = -delta;
					c=getc();
				}
				ungetchar(c);
				c = getname('z');
				if(delta){
					if(numeric){
						if(!alldigs(string[c].str))
							error('#');
						numset(c, qatoi(string[c].str)+delta);
					} else
						strinc(c, delta);
				}
                                uc.i=c;
				pushinp(i, uc, literal);
				literal = FALSE;
				c=getc();
				break;
			case BUF:
			case BFILEN:
				c = getname(cspec[i]);
				if(i == BUF)
					bbempty = TRUE;
				/* fall through */
			case TTY:
			case FILEN:
			case PAT:
			case RHS:
                                uc.i=c;
				pushinp(i, uc, literal);
				literal = FALSE;
				/* fall through */
			case NOTHING:
				c = getc();
				break;
			case CURBN:
				c = bname[curbuf-buffer];
				break;
			case ASCII:
				c=getucs(2);
				break;
			case UCS2:
				c=getucs(4);
				break;
			case UCS4:
				c=getucs(6);
				break;
			case BACKSLASH:
				qcount++;
				break;
			default:
				afterq = c;
				c = '\\';
				break;
			}
		}
	}
	lastc = c;
	return(c);
}

int
ttyc(void)
{
	byte b[4];
	byte *c;

	c=&b[0];

	initflag = 0;

	if(read(0,c,1)<=0)
		return lastttyc=EOF;

	if(convnutf(c,&lastttyc,1))
		return lastttyc;      /* unicode ascii */

	/*
	 * lastttyc should now hold the number of extra bytes needed,
         * or 0 if there was a decoding error.
	 */

	if(0==lastttyc)
		return lastttyc=EOF;  /* decoding error */

	if(read(0,++c,lastttyc)<=0)   /* read in required extra bytes of utf */
		return lastttyc=EOF;

	if(convutf(--c,&lastttyc))
		return lastttyc;

	return lastttyc=EOF;
}

int
posn(int c, int *s)
{
	int *is;
	is = s;
	while(*s)
		if(c == *s++)
			return(s - is - 1);
	return(-1);
}

void
pushinp(int type, union pint_t arg, int literal)
{
	struct stack *s;

	s = ++stackp;
	if(s == stack+STACKSIZE)
		error('l');
	stackp->literal = literal;
	if(tflag){
		putchr('[');
		if(literal)
			putchr('\'');
		putchr(cspec[type]);
	}
	s->type=type;
	s->charno = 0;
	switch(type){
	case BFILEN:
		s->strname = FILE(arg.i);
		break;
	case STRING:
		s->strname = arg.i;
		break;
	case FILEN:
		s->strname = savedfile;
		break;
	case PAT:
		s->strname = SAVPAT;
		break;
	case RHS:
		s->strname = SAVRHS;
		break;
	case BRWS:
		s->strname = BROWSE;
		break;
	case BUF:
		if((s->bufptr=buffer+arg.i) == curbuf && appflag)
			error('\\');
		s->lineno=1;
		break;
	case GLOB:
		s->globp = arg.p;
		break;
	}
	if(tflag){
		if(type==BFILEN || type==STRING || type==BUF)
			putchr(bname[arg.i]);
		putchr('=');
	}
}

void
popinp(void)
{
	if(stackp->type == BUF)
		bbempty = TRUE;
	if(tflag){
		putchr(']');
		flush();
	}
	--stackp;
}

int
gettty(void)
{
	int c, gf;
	int *p;
	p = linebuf;
	gf = stackp->type;
	while((c=getchar()) != '\n') {
		if (c==EOF) {
			if (gf == GLOB)
				peekc=c;
			return(c);
		}
		if (c == 0)
			continue;
		*p++ = c;
		if (p >= &linebuf[LBSIZE-2])
			error('l');
	}
	*p++ = 0;
	if (!oneline && linebuf[0]=='.' && linebuf[1]==0)
		return(EOF);
	return(0);
}

int
getquote(int *p, int (*f)())
{
	int c;
	c = (*f)();
	if(c == '\\') {
		if(peekc == 0)
			nextchar();	/* prime peekc */
		if(posn(peekc, p) >= 0) {
			c = escape(peekc);
			(*f)();		/* clear peekc */
		}
	}
	return(c);
}
