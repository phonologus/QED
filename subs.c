#include "qed.h"

int *next_new;
int *next_old;
int new_line;
int old_line;
int rhsbuf[RHSIZE];

void
substitute(int inglob, int reg)
{
	int n, m;
	int *p;
	int *q;
	addr_i a1;
	int gsubf;
	extern int getsub();
	int t, count, autop=0;

	count=0;
	t = FALSE;
	n=getnum();
	gsubf = compsub(TRUE, &autop);
	if(reg!= -1){	/* Substitution in a register */
		cpstr(string[reg].str, linebuf);
		loc2=0;	/* signal execute to take line from linebuf */
		a1=0;
		goto Do_it;
	}
	for (a1 = addr1; a1 <= addr2 && reg==-1; a1++) {
	Do_it:
		if (execute(a1)) {
			next_old=linebuf;
			next_new=genbuf;
			m=n;
			do {
				if (--m <= 0) {
					dosub();
					t = TRUE;
					count++;
					if (!gsubf)
			break;
				}
				/* can't find something at EOL twice */
				if (*loc2=='\0')
			break;
				/* can't match same location twice */
				if (loc1==loc2)
					loc2++;
			} while (execute((addr_t)0));
			if (m<=0) {
				inglob |= TRUE;
				p=next_old;
				do ; while (*p++);
				place(next_old,p,0);
				if(reg==-1){
					p=linebuf;
					q=genbuf;
					do ; while (*p++ = *q++);
					replace(a1,putline());
					m=append(getsub,a1);
					a1 += m;
					addr2 += m;
				}else{
					startstring();
					copystring(genbuf);
					setstring(reg);
				}
			}
		}
	}
	if (!inglob)
		error('s');
	settruth(t);
	setcount(count);
	if(autop)
		if(reg>=0)
			puts(string[reg].str);
		else{
			addr1=addr2=dot;
			display('p');
		}
}

int
compsub(int subbing, int *autop)
{
	int seof, c;
	int *rhsmagic;
	int *p;
	int getsvc();

	*autop=FALSE;
	seof = getchar();
	if(subbing) {
		compile(seof);
		rhsmagic = utfstr_rhsa;
	}
	else
		rhsmagic = utfstr_rhsb;
	rhsmagic[0] = seof;
	p = rhsbuf;
	startstring();
	for (;;) {
		c = getquote(rhsmagic, getsvc);
		if (c=='\n' || c==EOF){
			*autop=TRUE;
			ungetchar('\n');
			break;
		}
		if (c==seof)
			break;
		*p++ = c;
		if (p >= &rhsbuf[RHSIZE])
			error('l');
	}
	*p = 0;
	dropstring();
	setstring(SAVRHS);
	if (subbing && nextchar() == 'g') {
		getchar();	/* clear 'g' */
		return(1);
	}
	return(0);
}

int
getsub(void)
{
	int *p1, *p2;

	p1 = linebuf;
	if ((p2 = linebp) == 0)
		return(EOF);
	do ; while (*p1++ = *p2++);
	linebp = 0;
	return(0);
}

void
dosub(void)
{
	int c;
	int *p;

	place(next_old,loc1,0);
	next_old=loc2;
	p=rhsbuf;
	while (c = *p++) {
		if (c=='&' || (c == '^' && uflag))
			place(loc1,loc2,c=='^');
		else if (escaped(c) && unescape(c)>='1' && unescape(c)<'1'+nbra) {
			c=unescape(c);
			place(braslist[c-'1'],braelist[c-'1'], 0);
		} else {
			*next_new++ = c;
			if (next_new >= genbuf+LBSIZE)
				error('l');
		}
	}
}

void
place(int *l1, int *l2, int ucase)
{
	int *sp;
	int c;

	sp = next_new;
	while (l1 < l2) {
		*sp++ = *l1++;
		if (sp >= &genbuf[LBSIZE])
			error('l');
	}
	if(ucase){
		for(l1 = next_new;l1 < sp;){
			c = *l1;
			if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
				switch(uflag){
				case 's':
					*l1++ ^= 040;
					break;
				case 'u':
					*l1++ &= ~040;
					break;
				case 'l':
					*l1++ |= 040;
					break;
				default:
					l1++;
				}
			}else{
				l1++;
			}
		}
	}
	next_new = sp;
}

void
undo(void)
{
	addr_i l;

	for (l=zero+1; l<=dol && (core[l]|01)!=new_line; l++)
		;
	if (l>dol)
		error('u');
	replace(l,old_line);
	dot=l;
}

void
replace(addr_i line,addr_t ptr)
{
	addr_t *p;

	core[line] |= 01;
	for (p=names; p<names+NBUFS; p++)
		if (*p == core[line])
			*p = ptr|01;
	old_line = core[line];
	core[line] = ptr;
	new_line = ptr | 01;
}

void
join(void)
{
	addr_i l;
	int *p, *q;
	int rep;
	int autop=FALSE;

	rep=FALSE;
	if(nextchar() == '/'){
		compsub(FALSE, &autop);
		rep=TRUE;
	}
	p = genbuf;
	for (l=addr1; ;) {
		q = getline(core[l++], linebuf);
		while (*q) {
			*p++ = *q++;
			if (p >= genbuf + sizeof genbuf)
				error('l');
		}
		if(l > addr2)
			break;
		if(rep) {
			q = string[SAVRHS].str;
			while (*q) {
				*p++ = *q++;
				if (p >= genbuf + sizeof genbuf)
					error('l');
			}
		}
	}
	*p = '\0';
	linebp=p=linebuf;
	q=genbuf;
	do ; while (*p++ = *q++);
	getsub();
	core[(l=addr1++)]=putline();
	/* if you want marks preserved for join, change the above line to
	/* the one commented out here.
	/* problem: undo then undoes the join, but gets it wrong.  Your choice.
	replace(l=addr1++, putline());
	*/
	if(l != addr2)
		delete();
	append(getsub, l);
	if(autop){
		addr1=addr2=dot;
		display('p');
	}
}

int
next_col(int col,int *cp,int input)
{
	int c;

	c = *cp;
	if (c=='\t')
		col |= 07;
	else if (c<' ' || c=='\177')
		error('t'); /* invalid character in x data */
/*
	else
		if (input && (c==ttybuf.sg_erase || c==ttybuf.sg_kill))
			col++;	/* One column for the backslash */
	return (++col);
}

void
xform(void)
{
        union pint_t uc;
	int *i, *m, *o;
	addr_i line;
	int insert, change, ic, mc, c;
	int *tf, *tl;

	if(getchar() != '\n')
		error('x');
	for (line=addr1; line<=addr2; line++) {
		getline(core[line], linebuf);
		change=FALSE;
		dot=line;
		for(;;){
			puts(linebuf);
                        uc.i=0;
			pushinp(XTTY, uc, FALSE);
			m=rhsbuf;
			while ((c = getchar())!='\n') {
				if (c == EOF)
					error('t');  /* unexpected EOF */
				*m++ = c;
				if (m==rhsbuf+RHSIZE-1)
					error('l'); /* out of space */
			}
			*m='\0';
			if (m==rhsbuf)
				break;
			change++;
			i=linebuf;
			o=genbuf;
			do ; while (*o++ = *i++);
			if (i+(m-rhsbuf) > linebuf+LBSIZE)
				error('l'); /* out of space */
			i=genbuf;
			o=linebuf;
			m=rhsbuf;
			insert=FALSE;
			ic=0;
			mc=0;
			while (*i && *m && !insert) {
				if(*i=='\t' && *m!='#' && *m!='^' && *m!='$') {
					ic=next_col(ic,i,FALSE);
					tf=m;
					tl=m;
					do {
						if (*m!=' ' && *m!='\t') {
							if(*m=='%')
								*m=' ';
							tl=m+1;
						}
						mc=next_col(mc,m++,TRUE);
					} while (ic>mc && *m && *m!='#' &&
						 *m!='^' && *m!='$');
					if (ic>mc) {
						ic=mc;
						if (*m)
							tl=m;
					} else {
						if (tl==m)
							i++;
						else
							ic--;
					}
					while (tf!=tl)
						*o++ = *tf++;
				} else {
					mc=next_col(mc,m,TRUE);
					*o = *m;
					switch (*m++) {
					case ' ':
					case '\t':
						break;
					case '^':
						mc=ic;
						insert++;
						break;
					case '$':
						i=utfstr_nul;
						break;
					case '#':
						ic=next_col(ic,i++,FALSE);
						while(*m=='#' && ic>mc)
						      mc=next_col(mc,m++,TRUE);
						if (ic!=mc)
							error('t');
						break;
					case '%':
						*o = ' ';
						/* fall through */
					default:
						o++;
						ic=next_col(ic,i++,FALSE);
					} /* switch */
				} /* else */
				for (;;) {
					if (ic>mc && *m) {
						if (*m!=' ' && *m!='\t')
							error('t');
						mc=next_col(mc,m++,TRUE);
					} else if (mc>ic && *i) {
						ic=next_col(ic,i,FALSE);
						*o++ = *i++;
					} else
						break;
				}
			} /* while */
			if (mc>ic && m[-1]=='\t')
				*o++ = '\t';
			if (insert && (*o++ = *m++) == '\0') {
				replace(line,putline());
				linebp=i;
				append(getsub,line);
				line++;
				addr2++;
				change = FALSE;
			} else {
				while (*m)
					*o++ = *m++;
				do ; while (*o++ = *i++);
			}
		}
		if (change)
			replace(line,putline());
	} /* for */
}
