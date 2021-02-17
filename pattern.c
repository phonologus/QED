#include "qed.h"

enum {
  ESIZE = 128    /* ESIZE-1 must fit in a signed byte */
};

int	expbuf[ESIZE+4];
int	expgood	/*0*/;		/* flag indicating if compiled exp is good */

enum {
  STAR = 1,
  CCHR = 2,
  CDOT = 4,
  CCL = 6,
  NCCL = 8,
  CFUNNY = 10,
  CALT = 12,
  CBACK	= 14,
  CKET = 16,
  CDOL = 17,
  CEOF = 18,
  CBRA = 19,
  CBOI = 20,
  CEOI = 21,
  CSPACE = 22
};

#define STARABLE CBACK

int	circfl;
int	pmagic[] = {'/','.','$','^','*','+','\\','(',')','<','|','>','{','}',
                    '[','!','_','1','2','3','4','5','6','7','8','9','\0'};

void
compile(int eof)
{
	int c;
	int *ep, *penultep;
	int *lastep, *bracketp, bracket[NBRA];
	struct{
		int	*althd;		/* start of code for < ... > */
		int	*altlast;	/* start of code for last < or | */
		int	*bpstart;	/* bracketp at start of < and | */
		int	*bpend;		/* bracketp at end of > or | */
		int	nbstart;	/* nbra at start of < and | */
		int	nbend;		/* nbra at end of > or | */
		int	firstalt;	/* is this the first alternative? */
	} *asp, altstk[NBRA];

	if(eof == '\n')
		error('x');
	pmagic[0] = eof;
	if ((c=nextchar()) == eof || c=='\n') {
		if (!expgood)
			goto cerror;
		if(c!='\n')
			getchar();	/* eat the eof character */
		return;
	}
	expgood = FALSE;
	ep = expbuf;
	lastep = 0;
	bracketp = bracket;
	nbra = 0;
	asp = altstk-1;
	startstring();	/* for the saved pattern register */
	circfl = 0;
	if (c=='^') {
		getsvc();	/* save the caret */
		circfl++;
	}
	for (;;) {
		c = getquote(pmagic, getsvc);
		if (c==eof || c=='\n') {
			if (bracketp!=bracket || asp>=altstk)
				goto cerror;
			*ep++ = CEOF;
			expgood = TRUE;
			dropstring();	/* lose the eof character */
			setstring(SAVPAT);
			if(c=='\n')
				ungetchar(c);
			return;
		}
		if (ep >= &expbuf[ESIZE-5])
			goto cerror;
		penultep = lastep;
		lastep = ep;

		if(c != escape(eof)) switch (c) {
		case escape('('):
			if (nbra >= NBRA)
				goto cerror;
			*bracketp++ = nbra;
			*ep++ = CBRA;
			*ep++ = nbra++;
			continue;
		case escape(')'):
			if (bracketp <= bracket)
				goto cerror;
			*ep++ = CKET;
			*ep++ = *--bracketp;
			continue;
		case escape('{'):
			*ep++ = CBOI;
			continue;
		case escape('}'):
			*ep++ = CEOI;
			continue;
		case escape('_'):
			*ep++ = CSPACE;
			continue;
		case escape('!'):
			*ep++ = CFUNNY;
			continue;
		case '<':
			if (++asp >= &altstk[NBRA])
				goto cerror;
			*ep++ = CALT;
			asp->althd = ep;
			ep++;
			asp->bpstart = bracketp;
			asp->nbstart = nbra;
			asp->firstalt = TRUE;
			asp->altlast = ep++;
			lastep = 0;
			continue;
		case '|':
			if (asp<altstk)
				break;
			if (asp->firstalt) {
				asp->bpend = bracketp;
				asp->nbend = nbra;
			}
			if (bracketp!=asp->bpend || nbra!=asp->nbend)
				goto cerror;
			*ep++ = CEOF;
			asp->altlast[0] = ep-asp->altlast;
			asp->firstalt = FALSE;
			bracketp = asp->bpstart;
			nbra = asp->nbstart;
			asp->altlast = ep++;
			lastep = 0;
			continue;
		case '>':
			if (asp<altstk)
				break;
			if (!asp->firstalt &&
			    (bracketp!=asp->bpend || nbra!=asp->nbend))
				goto cerror;
			*ep++ = CEOF;
			asp->altlast[0] = ep-asp->altlast;
			lastep = asp->althd;
			*lastep = ep-lastep;
			lastep--;
			if (bracketp!=asp->bpstart || nbra!=asp->nbstart)
				lastep = 0;
			asp--;
			continue;
		case '*':
		case '+':
			if (penultep==0){
				*ep++ = CCHR;
				*ep++ = c;
			} else {
				if(*penultep>STARABLE)
					goto cerror;
				if(c == '+'){
					if((ep-penultep)+ep >= &expbuf[ESIZE-1])
						goto cerror;
					do
						*ep++ = *penultep++;
					while (penultep!=lastep);
				}
				*penultep |= STAR;
				lastep = 0;
			}
			continue;
		case '.':
			*ep++ = CDOT;
			continue;

		case '[':
			penultep = ep;
			*ep++ = CCL;
			*ep++ = 0;
			if ((c=getsvc()) == '^') {
				c = getsvc();
				ep[-2] = NCCL;
			}
			do {
				if (c == EOF || c == '\n')
					goto cerror;
				*ep++ = c;
				if ((lastc=getsvc()) == '-') {
					c=getsvc();
					if (c == EOF || c == '\n' || c<=ep[-1])
						goto cerror;
					ep[-1] |= ESC;
					*ep++ = c;
					lastc = getsvc();	/* prime lastc */
				} else if (dflag&&'a'<=(c|' ')&&(c|' ')<='z')
					*ep++ = c^' ';
				if (ep >= &expbuf[ESIZE-1])
					goto cerror;
			} while ((c=lastc) != ']');
			penultep[1] = ep-penultep-1;
			continue;


		case '$':
			if (nextchar() == eof || peekc=='\n') {
				*ep++ = CDOL;
				continue;
			}
			/* fall through */
		default:
			break;
		}
		/* if fell through switch, match literal character */
		/* Goddamned sign extension! */
		if (escaped(c) && unescape(c)>='1' && unescape(c)<='9') {
			*ep++ = CBACK;
			*ep++ = c-escape('1');
			continue;
		}
		c = unescape(c);
		if(dflag && (c|' ')>='a' && (c|' ')<='z'){
			*ep++ = CCL;
			*ep++ = 3;
			*ep++ = c;
			*ep++ = c^' ';
		}
		else{
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
   cerror:
	error('p');
}

int
getsvc(void)
{
	int c;
	addstring(c=getchar());
	return(c);
}

int
execute(addr_i addr)
{
	int *p1, *p2;

	if (addr==0) {
		if((p1=loc2) == 0)	/* G command */
			p1 = linebuf;
		else if (circfl)	/* not first search in substitute */
			return(FALSE);
	} else {
		if (addr==zero)
			return(FALSE);
		p1 = getline(core[addr], linebuf);
	}
	p2 = expbuf;
	if (circfl) {
		loc1 = p1;
		return(advance(p1, p2));
	}
	do {
		if (*p2 != CCHR  ||  p2[1] == *p1) {
			if (advance(p1, p2)) {
				loc1 = p1;
				return(TRUE);
			}
		}
	} while (*p1++);
	return(FALSE);
}

int
advance(int *lp, int *ep)
{
	int *curlp;
	int *althd, *altend;

	for (;;) {
		curlp = lp;
		switch (*ep++) {

		case CCHR:
			if (*ep++ == *lp++)
				continue;
			return(FALSE);

		case CCHR|STAR:
			do ; while (*lp++ == *ep);
			ep++;
			break;

		case CDOT:
			if (*lp++)
				continue;
			return(FALSE);

		case CDOT|STAR:
			do ; while (*lp++);
			break;

		case CCL:
		case NCCL:
			if (cclass(ep, *lp++, ep[-1]==CCL)) {
				ep += *ep;
				continue;
			}
			return(FALSE);

		case CCL|STAR:
		case NCCL|STAR:
			do ; while (cclass(ep, *lp++, ep[-1]==(CCL|STAR)));
			ep += *ep;
			break;

		case CFUNNY:
			if ((*lp>=' ' && *lp!='\177') || *lp=='\t' || *lp=='\0')
				return(FALSE);
			lp++;
			continue;

		case CFUNNY|STAR:
			while ((*lp<' ' && *lp && *lp!='\t') || *lp=='\177')
				lp++;
			lp++;
			break;

		case CBACK:
			if (braelist[*ep]==0)
				error('p');
			if (backref(*ep++, lp)) {
				lp += braelist[ep[-1]] - braslist[ep[-1]];
				continue;
			}
			return(FALSE);
	
		case CBACK|STAR:
			if (braelist[*ep] == 0)
				error('p');
			curlp = lp;
			while (backref(*ep, lp))
				lp += braelist[*ep] - braslist[*ep];
			while (lp >= curlp) {
				if (advance(lp, ep+1))
					return(TRUE);
				lp -= braelist[*ep] - braslist[*ep];
			}
			ep++;
			continue;

		case CBRA:
			braslist[*ep++] = lp;
			continue;

		case CKET:
			braelist[*ep++] = lp;
			continue;

		case CDOL:
			if (*lp==0)
				continue;
			return(FALSE);

		case CEOF:
			loc2 = lp;
			return(TRUE);

		case CBOI:
			if (alfmatch(*lp,0)
			    && (lp==linebuf || !alfmatch(lp[-1],1)))
				continue;
			return(FALSE);

		case CEOI:
			if (!alfmatch(*lp,1)
			    && lp!=linebuf && alfmatch(lp[-1],1))
				continue;
			return(FALSE);

		case CSPACE:
			if (*lp==' ' || *lp=='\t') {
				while (*lp == ' ' || *lp=='\t')
					lp++;
				continue;
				}
			return(FALSE);

		case CALT:
			althd = ep-1;
			altend = ep + *ep;
			for(ep++; ; ep+= *ep) {
				if(ep == altend)
					return(FALSE);
				if(advance(lp,ep+1) && advance(loc2,altend))
					return(TRUE);
			}

		case CALT|STAR:
			althd = ep-1;
			altend = ep + *ep;
			for(ep++; ep!=altend; ep+= *ep){
				if(advance(lp, ep+1)){
					if(loc2 == lp)
						break;
					if(advance(loc2, althd))
						return(TRUE);
				}
			}
			/* return (advance(lp,altend)) */
			continue;

		default:
			error('!');
		}
		/* star logic: executed by falling out of switch */
		do {
			lp--;
			if (advance(lp, ep))
				return(TRUE);
		} while (lp > curlp);
		return(FALSE);
	}
}

int
backref(int i, int *lp)
{
	int *bp;

	bp = braslist[i];
	while (*bp++ == *lp++)
		if (bp >= braelist[i])
			return(TRUE);
	return(FALSE);
}

int
alfmatch(int c,int tail)
{
	return (('a' <= c && c <= 'z')  ||
		('A' <= c && c <= 'Z')  ||
		(c == '_') ||
		(tail && '0' <= c && c<= '9'));
}

int
cclass(int *set, int c, int f)
{
	int n;
	if (c == 0)
		return(0);
	n = *set++;
	while (--n) {
		if (escaped(*set)) {
			if (unescape(*set++) <= c) {
				if (c <= *set++)
					return(f);
			} else
				set++;
			--n;
		} else if (*set++ == c)
			return(f);
	}
	return(!f);
}
