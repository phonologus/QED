/*
 * QED
 */

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

