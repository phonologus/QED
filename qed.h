/*
 * QED
 */

#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>  /* for open(), creat() and symbols for flags */
#include <sys/stat.h>  /* for open(), creat() and symbols for flags */
#include <fcntl.h>  /* for open(), creat() and symbols for flags */

#include <unistd.h>
#include <stdlib.h>

/*
 * utf and utfio
 */

enum {
  utfbytes=4,
  utfeof=-1,
  utfbsize=4096
};

typedef unsigned char byte;

byte utf8buff[utfbsize];   /* general purpose buffer for utf8 sequences */

#define convutf(p,z) convnutf(p,z,utfbytes)
#define convucode(p,z) convnucode(p,z,utfbytes)

int convnutf(byte *, int *, int);
int convnucode(int, byte *, int);


enum {
  UBSIZE=4096
};

struct utfio_s {
  int fd;          /* attached file */
  byte b[UBSIZE];  /* raw byte buffer */
  int i;           /* index of next unused byte, or UBSIZE if empty */
  int z;           /* index of EOF, or UBSIZE if EOF not yet in buffer */
};

typedef struct utfio_s utfio;

utfio *uioinit(int, utfio *);
int uioinitrd(utfio *);
int uiofill(utfio *);
int uioflush(utfio *);
int uioread(utfio *,int *,int);
int uiowrite(utfio *,int *,int);


/*
 * escaped chars
 *
 */

enum {
  ESC=0200,
  UNESC=ESC-1
};

#define escape(c) ((c)|ESC)
#define escaped(c) ((c)&ESC)
#define unescape(c) ((c)&UNESC)

/* types 
 *
 * addr_t : the address cookie
 * addr_i : a pointer/index to an addr_t
 *
 */

typedef int addr_t; 
typedef int addr_i; 

typedef unsigned char byte;

#include "vars.h"

/* address.c */

addr_i address(void);

/* blkio.c */

void initio(void);
int *getline(addr_t tl, int *lbuf);
addr_t putline(void);
void blkio_r(int b, int *buf);
void blkio_w(int b, int *buf);

/* com.c */

void jump(void);
void stacktype(int);
void getlabel(void);
addr_i looper(addr_i,addr_i,int *,int);
void search(int);
void setapp(void);
int append(int (*)(), addr_i);
void bcom(void);
void delete(void);
void allnums(void);
void numcom(int);
int condition(int, int, int, int);
void numset(int, int);
void numbuild(int);
void strcom(int);
void strinc(int, int);
int locn(int *, int *);
void ncom(int);
void allstrs(void);
void clean(int);

/* getchar.c */

int getnum(void);
int getsigned(void);
int qatoi(int *);
int alldigs(int *);
int getname(int);
int getaz(int);
int getnm(int, int (*)());
int getchar(void);
int getc(void);
int ttyc(void);
int posn(int, int *);
void pushinp(int, union pint_t, int);
void popinp(void);
int gettty(void);
int getquote(int *, int (*)());

/* getfile.c */
int newfile(int, int, int *);
void exfile(void);
int getfile(void);
void putfile(void);
void Unix(int);

/* glob.c */

void until(int, int);
void global(int);
void globuf(int);
void getglob(int[]);
int exglob(int *, int *);

/* main.c */

void rescue(int);   /* signal handler */
char *filea(void);
char *fileb(void);
void savall(void);
void restor(void);
void interrupt(int);  /* signal handler */
void commands(void);
void setreset(int*);
void delall(void);

/* misc.c */

void quit(void);
void bufinit(addr_i);
void chngbuf(int);
void newbuf(int);
void fixbufs(int);
void syncbuf(void);
void error(int);
void init(void);
void comment(void);
int abs(int);
void settruth(int);
void setcount(int);
int truth(void);
void modified(void);

/* move.c */

void move(int);
void fixup(int, int, int);
void reverse(addr_i,addr_i);
int getcopy(void);

/* pattern.c */

void compile(int);
int getsvc(void);
int execute(addr_i);
int advance(int *, int *);
int backref(int, int *);
int alfmatch(int, int);
int cclass(int *, int, int);

/* putchar.c */

typedef	unsigned long ulong;

void putdn(int);
void putlong(ulong);
void putl(int *);
void puts(int *);
void display(int);
void putct(int);
void putchar(int);
void flush(void);

/* setaddr.c */

void setdot(void);
void setall(void);
void setnoaddr(void);
void nonzero(void);

/* string.c */

int length(int *);
void startstring(void);
void addstring(int);
void dropstring(void);
void cpstr(int *, int *);
void shiftstring(int);
void clearstring(int);
void copystring(int *);
int eqstr(int *, int *);
void dupstring(int);
void setstring(int);
void strcompact(void);

/* subs.c */

void substitute(int, int);
int compsub(int, int*);
int getsub(void);
void dosub(void);
void place(int *, int *, int);
void undo(void);
void replace(addr_i,int);
void join(void);
int next_col(int,int *,int);
void xform(void);

