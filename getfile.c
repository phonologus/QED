#include "qed.h"
int	*nextip;
long	count;
/*
 * newfile: read a new file name (if given) from command line.
 *
 *	nullerr: if true, error if file is null.
 *	savspec: decides if string[savedfile] is set.
 *		If savspec==SAVEIFFIRST, only save if deffile is null.
 *	deffile: default file name.
 *
 *	return value: string[FILEBUF]!=deffile
 *		      (unless SAVEIFFIRST && deffile null ==> return FALSE)
 *	side effects:
 *		can set string[savedfile]
 *		always sets string[FILEBUF]
 *		zeroes count
 */

int
newfile(int nullerr, int savspec, int *deffile)
{
	int c;

	count = 0L;
	cpstr(deffile, genbuf);	/* in case we strcompact() */
	startstring();
	c = getchar();
	if(c == '\n')
		copystring(genbuf);
	else {
		if(c != ' ')
			error('f');
		do  c = getchar();  while(c == ' ');
		while(posn(c, utfstr_whitespace) < 0){
			if(c<' ' || c=='\177')
				error('f');
			addstring(c);
			c = getchar();
		}
	}
	setstring(FILEBUF);
	if(nullerr && string[FILEBUF].str[0]=='\0')
		error('f');
	if(savspec==SAVEALWAYS || (savspec==SAVEIFFIRST && genbuf[0]=='\0')){
		startstring();
		dupstring(FILEBUF);
		setstring(savedfile);
		if(savspec==SAVEIFFIRST && genbuf[0]=='\0')
			return(FALSE);
	}
	return(!eqstr(genbuf, string[FILEBUF].str));
}

void
exfile(void)
{
	close(io);
	io = -1;
	if (vflag && initflag)
		ncom('f');
	else if (vflag) {
		putlong(count);
		putchr('\n');
	}
	setcount((int)count);
}

int
getfile(void)
{
	int c;
	int *lp, *fp;
	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			ninbuf = uioread(uio, genbuf, LBSIZE)-1;
			if (ninbuf < -1) {
				error('r');
			} else if(ninbuf < 0) {
				if(lp != linebuf) {
					*genbuf='\n';
					*(genbuf+1)=utfeof;
					putstr(utfstr_nappended);
				} else {
					return(EOF);
				}
			}
			fp = genbuf;
		}
		if (lp >= &linebuf[LBSIZE])
			error('l');
		if ((*lp++ = c = *fp++ ) == 0) {
			lp--;
			continue;
		}
		count++;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	return(0);
}

void
putfile(void)
{
	addr_i a1;
	int *fp, *lp;
	int nib;
	nib = LBSIZE;
	fp = genbuf;
	a1 = addr1;
	if(a1 == zero)
		a1++;
	while(a1 <= addr2){
		lp = getline(core[a1++], linebuf);
		for(;;){
			if (--nib < 0) {
				if(uiowrite(uio, genbuf, fp-genbuf) < 0)
					error('w');
				nib = LBSIZE-1;
				fp = genbuf;
			}
			count++;
			if ((*fp++ = *lp++) == 0) {
				fp[-1] = '\n';
				break;
			}
		}
	}
	uiowrite(uio, genbuf, fp-genbuf);
	uioflush(uio);    /* Don't forget to flush */
}

void (*savint)(int);	/* awful; this is known in error() */
int savintf = -1;

void
Unix(int type)
{
	int n;
        union pint_t uc;
	int pid, rpid;
	int *s;
	int c;
	addr_i a;
	void (*onpipe)(int);
	int retcode;
	int	unixbuf[512];
	int	pipe1[2];
	int	pipe2[2];
	addr_i a1, a2, ndot;
	startstring();	/* for the \zU register */
	if(type == '!')
		setnoaddr();
	else {
		if(type == '>' || type == '|')
			nonzero();
		count = 0L;
		if(pipe(pipe1) == -1){
			lastc = '\n';
			error('|');
		}
	}
	/* Quick hack: if char is doubled, push \'zU */
	if(nextchar()==type){
		getchar();	/* throw it away */
                uc.i=UNIX;
		pushinp(STRING, uc, TRUE);
	}
	/*
	 * Use c not *s as EOF and getchar() are int's
	 */
	for(s=unixbuf;(c=getquote(utfstr_nl, getsvc))!='\n' && c!=EOF;*s++=unescape(c)){
		if(s>=unixbuf+512)
			error('l');
	}
	dropstring();	/* drop the newline */
	setstring(UNIX);
	*s='\0';
	a1 = addr1;
	a2 = addr2;
	ndot = dot;  /* silence compiler warning */
	if ((pid = fork()) == 0) {
		signal(SIGHUP, onhup);
		signal(SIGINT, onintr);
		signal(SIGQUIT, onquit);
		if(type=='<' || type=='|'){
			close(1);
			dup(pipe1[1]);
		}else if(type == '>'){
			close(0);
			dup(pipe1[0]);
		}
		if (type != '!') {
			close(pipe1[0]);
			close(pipe1[1]);
		}
		if(type == '|'){
			if(pipe(pipe2) == -1){
				putstr(utfstr_querypipe);
				lasterr=1;
				quit();
			}
			if((pid=fork()) == 0){
				close(1);
				dup(pipe2[1]);
				close(pipe2[0]);
				close(pipe2[1]);
				tfile = tfile2;	/* ugh */
				/*
				 * It's ok if we get SIGPIPE here
				 */
				display('p');
				lasterr=0;
				quit();
			}
			if(pid == -1){
				putstr(utfstr_cantforkqueryshriek);
				lasterr=1;
				quit();
			}
			close(0);
			dup(pipe2[0]);
			close(pipe2[0]);
			close(pipe2[1]);
		}
		if (*unixbuf)
			execl("/bin/sh", "sh", "-c", utf8(unixbuf), (char *)NULL);
		else
			execl("/bin/sh", "sh", (char *)NULL);
		lasterr=-1;
		quit();
	}
	if(pid == -1){
		putstr(utfstr_cantfork);
		error('!');
	}
	savint = signal(SIGINT, SIG_IGN);
        savintf++;
	if(type=='<' || type=='|') {
		close(pipe1[1]);
		io = pipe1[0];
		uioinitrd(io,uio);
		ninbuf = 0;
		append(getfile,addr2);
		close(io);
		io = -1;
		ndot = dot;
	} else if(type == '>') {
		onpipe = signal(SIGPIPE, SIG_IGN);
		close(pipe1[0]);
		uioinit(pipe1[1],uio);
		a=addr1;
		do{
			s=getline(core[a++], linebuf);
			do; while(*s++);
			*--s='\n';
			n=utf8nstring(linebuf,utf8buff,s-(linebuf-1));
			if (write(pipe1[1],utf8buff,n)<0){
				putstr(utfstr_queryo);
				break;
			}
		}while (a<=addr2);
		close(pipe1[1]);
		signal(SIGPIPE, onpipe);
	}
	while ((rpid = wait(&retcode)) != pid && rpid != -1);
	retcode = (retcode>>8)&0377;
	settruth(retcode);
	signal(SIGINT, savint);
	if(type == '|'){
		if(retcode == 0){
			addr1 = a1;
			addr2 = a2;
			delete();
			dot = ndot - (a2-a1+1);
		} else
			error('0');
	}
	if(vflag)
		putstr(utfstr_shriek);
}
