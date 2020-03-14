/*
 * QED
 */

#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>  /* for open(), creat() and symbols for flags */
#include <sys/stat.h>  /* for open(), creat() and symbols for flags */
#include <fcntl.h>  /* for open(), creat() and symbols for flags */

#include <stdlib.h>

#include "utf.h"

/* types
 * 
 * addr_t : the address cookie
 * addr_i : a pointer/index to an addr_t
 *
 */

typedef int addr_t; 
typedef int addr_i; 

#include "vars.h"

/* address.c */

addr_i address(void);

/* blkio.c */

void initio(void);
char *getline(addr_t tl, char *lbuf);
addr_t putline(void);
void blkio(int b, char *buf, int (*iofcn)());

/* com.c */

void jump(void);
void stacktype(int);
void getlabel(void);
addr_i looper(addr_i,addr_i,char *,int);
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
int locn(char *, char *);
void ncom(int);
void allstrs(void);
void clean(int);

/* getchar.c */

int getnum(void);
int getsigned(void);
int qatoi(char *);
int alldigs(char *);
int getname(int);
int getaz(int);
int getnm(int, int (*)());
int getchar(void);
int getc(void);
int ttyc(void);
int posn(char, char *);
void pushinp(int, union pint_t, int);
void popinp(void);
int gettty(void);
int getquote(char *, int (*)());

/* getfile.c */
int newfile(int, int, char *);
void exfile(void);
int getfile(void);
void putfile(void);
void Unix(char);

/* glob.c */

void until(int, int);
void global(int);
void globuf(int);
void getglob(char[]);
int exglob(char *, char *);

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

void compile(char);
int getsvc(void);
int execute(addr_i);
int advance(char *, char *);
int backref(int, char *);
int alfmatch(char, int);
int cclass(char *, int, int);

/* putchar.c */

typedef	unsigned long ulong;

void putdn(int);
void putlong(ulong);
void putl(char *);
void puts(char *);
void display(int);
void putct(int);
void putchar(char);
void flush(void);

/* setaddr.c */

void setdot(void);
void setall(void);
void setnoaddr(void);
void nonzero(void);

/* string.c */

int length(char *);
void startstring(void);
void addstring(int);
void dropstring(void);
void cpstr(char *, char *);
void shiftstring(int);
void clearstring(int);
void copystring(char *);
int eqstr(char *, char *);
void dupstring(int);
void setstring(int);
void strcompact(void);

/* subs.c */

void substitute(int, int);
int compsub(int, int*);
int getsub(void);
void dosub(void);
void place(char *, char *, int);
void undo(void);
void replace(addr_i,int);
void join(void);
int next_col(int,char *,int);
void xform(void);

