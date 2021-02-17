#include "qed.h"

int	col;

void
putdn(int i)
{
	putlong((ulong)i);
	putchr('\n');
}

void
putlong(ulong i)
{
	int r;
	r = i%10;
	i /= 10;
	if(i)
		putlong(i);
	putchr('0'+r);
}

void
putl(int *sp)
{
	listf++;
	putstr(sp);
	listf = FALSE;
}

void
putstr(int *sp)
{
	col = 0;
	while (*sp)
		putchr(*sp++);
	putchr('\n');
}

void
display(int lf)
{
	addr_i a1;
	int r;
	int *p;
	int i;
	int nf;
	listf = (lf == 'l' || lf == 'L');
	nf = (lf == 'P' || lf == 'L');
	lf = listf;
	setdot();
	nonzero();
	a1 = addr1;
	r = a1 - zero;
	do{
		col = 0;
		if(nf){
			putlong((ulong)r++);
			for(i=0; i<NBUFS; i++)
				if(mark(core[a1]) == names[i]){
					putchr('\'');
					putchr(bname[i]);
				}
			listf = 0;
			putchr('\t');
			col = 8;
			listf = lf;
		}
		for(p = getline(core[a1++],linebuf);*p;putchr(*p++));
		putchr('\n');
	}while (a1 <= addr2);
	dot = addr2;
	listf = FALSE;
}

void
putct(int c)
{
	putchr(c);
	putchr('\t');
}

void
putchr(int c)
{
	int *lp;

	lp = linp;
	if (listf) {
		if (c=='\n') {
			if(linp!=line && linp[-1]==' ') {
				*lp++ = '\\';
				*lp++ = 'n';
			}
		} else {
			if (col >= (LINELEN-BELL)) {
				*lp++ = '\\';
				*lp++ = '\n';
				*lp++ = '\t';
				col = 8;
			}
			col++;
			if (c=='\b' || c=='\\' || c=='\t') {
				*lp++ = '\\';
				c = c=='\b'? 'b' : c=='\t'? 't' : '\\';
				col++;
			} else if (escaped(c)) {
				*lp++ = '\\';
				c=unescape(c);
				col++;
			} else if (c<' ' || c=='\177') {
				*lp++ = '\\';
				*lp++ = 'x';
				*lp++ = hex[(c>>4)&0xF];
				c     = hex[c&0xF];
				col += 3;
			} else if (c>'\177' && c<=0xFFFF) {
				*lp++ = '\\';
				*lp++ = 'u';
				*lp++ = hex[(c>>12)&0xF];
				*lp++ = hex[(c>>8)&0xF];
				*lp++ = hex[(c>>4)&0xF];
				c     = hex[c&0xF];
				col += 5;
			} else if (c>0xFFFF) {
				*lp++ = '\\';
				*lp++ = 'U';
				*lp++ = hex[(c>>20)&0xF];
				*lp++ = hex[(c>>16)&0xF];
				*lp++ = hex[(c>>12)&0xF];
				*lp++ = hex[(c>>8)&0xF];
				*lp++ = hex[(c>>4)&0xF];
				c     = hex[c&0xF];
				col += 7;
			}
		}
	}
	*lp++ = c;
	if(c == '\n' || lp >= &line[LINELEN-BELL]) {
		linp = lp;
		flush();
		lp = linp;
	}
	linp = lp;
}

void
flush(void)
{
	int n;
	if(linp != line){
		n=utf8nstring(line,utf8buff,linp-line);
		write(1, utf8buff, n); 
		linp = line;
	}
}
