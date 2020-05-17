/*
 * QED
 */

/*
 * utf
 */

extern utfio _uio;
extern utfio *uio;

extern jmp_buf	savej;

extern addr_t *core;

enum {
	LINELEN=70
};

enum {
	LDCHUNK=4096
};

enum {
  TRUE = 1,
  FALSE = 0
};

#define	LBSIZE	4096
#define RHSIZE	(LBSIZE/4)	/* ed says /2; but that's unreasonable! */
#define	NBRA	9
#define	EOF	(-1)
#define	FILERR	0200

enum {
  SAVENEVER = 0,
  SAVEIFFIRST = 1,
  SAVEALWAYS = 2
} ; 

/*
 * Stack types.  Must appear in the order as in cspec[]/getchar.
 * XTTY, GLOB and BRWS are special types with no corresponding special character.
 * Special is never used directly - it is just used in tracing
 *	pushinp()/getchar.c can be called with a negative index for cspec[]
 */
extern int special[]; /*="xgBbBcfFlprzN\"\\'"*/
#define	cspec	(special + 3)

enum {
  XTTY = 0175,
  GLOB = 0176,
  BRWS = 0177
} ;

enum {
  BUF=0,
  CURBN=1,
  QUOTE=2,
  FILEN=3,
  BFILEN=4,
  TTY=5,
  PAT=6,
  RHS=7,
  STRING=8,
  NEWL=9,
  NOTHING=10,
  BACKSLASH=11,
  LITERAL=12
};

/*
 * Getchar-linked macros
 */
#define ungetchar(c)	(peekc = (c))
#define nextchar()	(peekc = getchar())

extern int hex[];

extern int	bname[]; /*="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~"*/

enum {
  NBUFS=56
};

/*
 * The buffer structure.  All info associated with each buffer stored here
 */
struct buffer{
	addr_i zero;
	addr_i dot;
	addr_i dol;
	char cflag;
	char gmark;
};

extern struct buffer buffer[NBUFS];
extern struct buffer *curbuf;

/*
 * The string structure
 * The first NBUFS strings are the registers
 * The next NBUFS are the file names
 * The next two are the saved pattern and saved right hand side
 * The next is the special register for browsing (the `ob' command)
 * The next is a file buffer.
 * Strings are stored in the `strarea' area and garbage collected
 * when the area is full.  The NSTRCHARS parameter can be increased
 * if you've got lots of core.
 * The first two characters of `strarea' form the null string.
 * The third and subsequent characters are the storage for non null
 * strings.
 * The null string has to be stored in this area so that
 * the R and S commands write out valid pointers to the null string.
 * The last entry in the string structure is used to store the pointer
 * to the next free position.
 * In string.c strfree is defined as "string[NSTRING].str".
 */
#define	NSTRING	(NBUFS+NBUFS+4)
#define	COUNT	(26+'C'-'A')
#define	TRUTH	(26+'T'-'A')
#define	UNIX	(26+'U'-'A')
#define	FILE(z)	(NBUFS+(z))
#define	SAVPAT	(NBUFS+NBUFS)
#define	SAVRHS	(SAVPAT+1)
#define	BROWSE	(SAVRHS+1)
#define	FILEBUF	(BROWSE+1)

struct string{
	int len;
	int *str;
};

#define NSTRCHARS 1024

extern struct string string[NSTRING+1];
extern int strarea[NSTRCHARS + 2];

#define	nullstr strarea
#define	strchars (&strarea[2])
#define STACKSIZE 16
/*
 * The getchar stack.
 */

union pint_t{
  int *p;
  int i;
};
 
struct stack{
	int type;
	int literal;
	union{
		struct buffer *u1bufptr;
		int *u1globp;
	}u1;
	union{
		int u2lineno;
		int u2strname;
	}u2;
	int charno;
};

extern struct stack stack[STACKSIZE];
extern struct stack *stackp;

#define	bufptr	u1.u1bufptr
#define	globp	u1.u1globp
#define	lineno	u2.u2lineno
#define	strname	u2.u2strname


extern int	peekc;
extern int	lastc;
extern int	line[LINELEN];
extern int	*linp;
extern int	savedfile;
extern int	linebuf[LBSIZE];
extern addr_i	zero;
extern addr_i	dot;
extern addr_i	dol;
extern addr_i	lastdol;
extern addr_i	endcore;
extern addr_i	fendcore;
extern addr_i	addr1;
extern addr_i	addr2;
extern int	genbuf[LBSIZE];
extern int	*linebp;
extern int	ninbuf;
extern int	io;
extern void	(*onhup)(int);
extern void	(*onquit)(int);
extern void	(*onintr)(int);
extern int	lasterr;
#define	PAGESIZE	22
extern	int pagesize;
extern int bformat;	/* = 'p' */
extern int	appflag;
extern int	cflag;
extern int	cprflag;
extern int	dflag;
extern int	eflag;
extern int	gflag;
extern int	biggflag;
extern int	iflag;
extern int	prflag;
extern int	tflag;
extern int	uflag;
extern int	vflag;
extern int	qok;
extern int	eok;
extern int	initflag;
extern int	nestlevel;
extern int	lastttyc;
extern int	listf;
extern int	tfile;
extern int	tfile2;
extern char	tfname[];
extern int	*loc1;
extern int	*loc2;
extern int	names[NBUFS];
extern int	*braslist[NBRA];
extern int	*braelist[NBRA];
extern int	nbra;
extern int	oneline;
extern int	lchars[]; /*= "pPlL"*/
extern int	bbempty;	/* whether getc's internal buffer buffer needs reloading */
int	*getline();
addr_i	address();

