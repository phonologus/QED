#include "qed.h"

typedef	unsigned long ulong;

int	col;

void
putdn(int i)
{
	putlong((ulong)i);
	putchar('\n');
}

void
putlong(ulong i)
{
	int r;
	r = i%10;
	i /= 10;
	if(i)
		putlong(i);
	putchar('0'+r);
}

void
putl(int *sp)
{
	listf++;
	puts(sp);
	listf = FALSE;
}

void
puts(int *sp)
{
	col = 0;
	while (*sp)
		putchar(*sp++);
	putchar('\n');
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
	r = (a1 - zero) & 077777;
	do{
		col = 0;
		if(nf){
			putlong((ulong)r++);
			for(i=0; i<NBUFS; i++)
				if((core[a1]|01) == names[i]){
					putchar('\'');
					putchar(bname[i]);
				}
			listf = 0;
			putchar('\t');
			col = 8;
			listf = lf;
		}
		for(p = getline(core[a1++],linebuf);*p;putchar(*p++));
		putchar('\n');
	}while (a1 <= addr2);
	dot = addr2;
	listf = FALSE;
}

void
putct(int c)
{
	putchar(c);
	putchar('\t');
}

void
putchar(int c)
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
			if (col >= (LINELEN-4-2)) {
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
			} else if (escaped(c) || c<' ' || c=='\177') {
				*lp++ = '\\';
				*lp++ = ((c>>6)&03)+'0';
				*lp++ = ((c>>3)&07)+'0';
				c     = ( c    &07)+'0';
				col += 3;
			}
		}
	}
	*lp++ = c;
	if(c == '\n' || lp >= &line[LINELEN-2-4]) {
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
