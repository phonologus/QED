/*
 * QED
 */

#include "debug.h" /* REMOVE ME */

#include "vars.h"

/* address.c */

int *address(void);

/* blkio.c */

void initio(void);
char *getline(int tl, char *lbuf);
int putline(void);
void blkio(int b, char *buf, int (*iofcn)());

/* com.c */

void jump(void);
void stacktype(int);
void getlabel(void);
int* looper(int *,int *,char *,int);
void search(int);
void setapp(void);
int append(int (*)(), int *);
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
int atoi(char *);
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

void rescue(void);
char *filea(void);
char *fileb(void);
void savall(void);
void restor(void);
void interrupt(void);
void unlock(void);
void commands(void);
void setreset(int *);
void delall(void);

/* misc.c */

void bufinit(int *);
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
void reverse(int *,int *);
int getcopy(void);

/* pattern.c */

void compile(char);
int getsvc(void);
int execute(int *);
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
int compsub(int, int *);
int getsub(void);
void dosub(void);
void place(char *, char *, int);
void undo(void);
void replace(int *,int);
void join(void);
int next_col(int,char *,int);
void xform(void);

