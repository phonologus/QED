#include "qed.h"

#include <sys/ioctl.h>  /* for FIOCLEX */

char tfname[]="/tmp/qxxxxx";

enum {
  SIGINT = 2
};

void
bufinit(int *n)
{
	struct buffer *bufp;
	int *fend;
	fend=n;
	for(bufp=buffer;bufp!=buffer+NBUFS;bufp++){
		bufp->zero=fend;
		bufp->dot=fend;
		bufp->dol=fend;
		bufp->cflag = FALSE;
	}
}

void
chngbuf(int bb)
{
	syncbuf();
	newbuf(bb);
	savedfile = FILE(curbuf-buffer);
}

void
newbuf(int bb)
{
	curbuf = buffer+bb;
	zero=curbuf->zero;
	dot=curbuf->dot;
	dol=curbuf->dol;
	cflag = curbuf->cflag;
}

void
fixbufs(int n)
{
	struct buffer *bufp;
	lock++;
	for(bufp=curbuf+1;bufp!=buffer+NBUFS;bufp++){
		bufp->zero+=n;
		bufp->dot+=n;
		bufp->dol+=n;
	}
	unlock();
}

void
syncbuf(void)
{
	/* curbuf->zero = zero;	/* we never assign to it, so needn't save */
	curbuf->dot=dot;
	curbuf->dol=dol;
	curbuf->cflag = cflag!=FALSE;	/* Normalize to fit in a char */
}

void
error(int code)
{
	extern int savint;	/* error during a ! > < | ?? */
	unlock();
	if(code){
		for(;stackp != stack ;--stackp)
			if(stackp->type == BUF || stackp->type == STRING){
				putchar('?');
				if(stackp->type == BUF){
					putchar('b');
					putchar(bname[stackp->bufptr - buffer]);
					putlong((long)stackp->lineno);
					putchar('.');
				}else{
					putchar('z');
					putchar(bname[stackp->lineno]);
				}
				putlong((long)stackp->charno);
				putchar(' ');
			}
		putchar('?');
		putchar(lasterr=(code&~FILERR));
		if(code&FILERR){
			putchar(' ');
			putl(string[FILEBUF].str);
		} else
			putchar('\n');
	}
	if(eflag && code)
		exit(code);
	nestlevel = 0;
	listf = FALSE;
	gflag = FALSE;
	biggflag = FALSE;
	stackp = stack;
	peekc = 0;
	if(code && code!='?'){		/* if '?', system cleared tty input buf */
		while(lastttyc!='\n' && lastttyc!=EOF){
			getchar();
		}
	}
	lseek(0, 0L, 2);
	if (io > 0) {
		close(io);
		io = -1;
		cflag = TRUE;	/* well, maybe not, but better be safe */
	}
	appflag=0;
	if(savint>=0){
		signal(SIGINT, savint);
		savint= -1;
	}
	reset();
}

void
init(void)
{
	char *p;
	int pid;
	lock++;
	close(tfile);
	pid = getpid();
	for (p = &tfname[11]; p > &tfname[6];) {
		*--p = (pid%10) + '0';
		pid /= 10;
	}
	close(creat(tfname, 0600));
	tfile = open(tfname, 2);
	tfile2 = open(tfname, 2);
	ioctl(tfile, FIOCLEX, 0);
	ioctl(tfile2, FIOCLEX, 0);
	brk(fendcore);
	bufinit(fendcore);
	newbuf(0);
	lastdol=dol;
	endcore = fendcore - 2;
	stackp=stack;
	stackp->type=TTY;
	unlock();
}

void
comment(void)
{
	int c, mesg;

	c = getchar();
	mesg = 0;
	if(c == '\"') {
		mesg++;
		c = getchar();
	}
	while(c != '\n' && c != '\"') {
		if(mesg)
			putchar(c);
		c = getchar();
	}
	if(mesg) {
		if(c == '\n')
			putchar(c);
		flush();
	}
}

int
abs(int n)
{
	return(n<0?-n:n);
}
/*
 * Slow, but avoids garbage collection
 */
void
settruth(int t)
{
	if(atoi(string[TRUTH].str) != t)
		numset(TRUTH, t);
}

void
setcount(int c)
{
	if(atoi(string[COUNT].str) != c)
		numset(COUNT, c);
}

int
truth(void)
{
	return(atoi(string[TRUTH].str) != FALSE);
}

void
modified(void)
{
	cflag=TRUE;
	eok=FALSE;
	qok=FALSE;
}
