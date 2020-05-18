#include "qed.h"

char tfname[]="/tmp/qxxxxx";

void
quit(void)
{
  free(core);
  unlink(tfname);
  exit(lasterr);
}

void
bufinit(addr_i n)
{
	struct buffer *bufp;
	addr_i fend;
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
	for(bufp=curbuf+1;bufp!=buffer+NBUFS;bufp++){
		bufp->zero+=n;
		bufp->dot+=n;
		bufp->dol+=n;
	}
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
	extern void (*savint)(int);	/* error during a ! > < | ?? */
        extern int savintf;
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
	if(eflag && code) {
		lasterr=code;
		quit();
	}
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
	lseek(0, 0L, SEEK_END);
	if (io > 0) {
		close(io);
		io = -1;
		cflag = TRUE;	/* well, maybe not, but better be safe */
	}
	appflag=0;
	if(savintf>=0){
		signal(SIGINT, savint);
		savintf = -1;
	}
	longjmp(savej,1);
}

void
init(void)
{
	char *p;
	int pid;
	if(tfile > 0)
		close(tfile);
	pid = getpid();
	for (p = &tfname[11]; p > &tfname[6];) {
		*--p = (pid%10) + '0';
		pid /= 10;
	}
	close(creat(tfname, 0600));
	tfile = open(tfname, O_RDWR);
	tfile2 = open(tfname, O_RDWR);
	/* (re-)initialize core.
	 * core was initialized to 0 on program entry.
	 * init() is also called by restore(), possibly with
	 * a core already allocated, so we need to free it
	 * before re-initializing.
	 */
	free(core);
	if ((core=(addr_t*)malloc(sizeof(addr_t)))==(addr_t *)0) 
		error('c');
	fendcore=0;
	endcore=fendcore-1;
	bufinit(fendcore);
	newbuf(0);
	lastdol=dol;
	stackp=stack;
	stackp->type=TTY;
	uio=&_uio;
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
	if(qatoi(string[TRUTH].str) != t)
		numset(TRUTH, t);
}

void
setcount(int c)
{
	if(qatoi(string[COUNT].str) != c)
		numset(COUNT, c);
}

int
truth(void)
{
	return(qatoi(string[TRUTH].str) != FALSE);
}

void
modified(void)
{
	cflag=TRUE;
	eok=FALSE;
	qok=FALSE;
}
