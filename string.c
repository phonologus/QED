#include "qed.h"

#define	strfree string[NSTRING].str
char *strstart;

int
length(char *s)
{
	char *t;
	if((t=s)==0)
		return(0);
	do;while(*t++);
	return(t-s-1);
}

void
startstring(void)
{
	strstart=strfree;
}

void
addstring(int c)
{
	if(strfree==strchars+NSTRCHARS)
		strcompact();
	*strfree++ = c;
}

void
dropstring(void)
{
	--strfree;
}

void
cpstr(char *a, char *b)
{
	do;while (*b++ = *a++);
}

void
shiftstring(int up)
{	/* A highly machine-dependent routine */
	struct string *sp;
	for(sp=string; sp<=string+NSTRING; sp++)
		if(up)
			sp->str += (int)strarea;
		else
			sp->str -= (int)strarea;
}

void
clearstring(int z)
{
	string[z].len = 0;
	string[z].str = nullstr;
}

void
copystring(char *s)
{
	while(*s)
		addstring(*s++);
}

int
eqstr(char *a, char *b)
{
	while(*a)
		if(*a++ != *b++)
			return(FALSE);
	return(*b=='\0');
}
/*
 * dupstring duplicates a string.
 * Because we may strcompact(), we do it first if necessary.
 */
void
dupstring(int z)
{
	if(strfree+string[z].len > strchars+NSTRCHARS)
		strcompact();	/* if insufficient, will get error when we copystring() */
	copystring(string[z].str);
}

void
setstring(int n)
{
	addstring('\0');
	if((string[n].len = length(strstart)) == 0)
		string[n].str = nullstr;
	else
		string[n].str = strstart;
	if(strfree >= strchars + NSTRCHARS)
		strcompact();
}

void
strcompact(void)
{
	struct string *cursor;
	struct string *thisstr;
	char *s, *t;
	s=strchars;
	for(;;){
		t=strchars+NSTRCHARS;
		for(cursor=string;cursor!=string+NSTRING;cursor++)
			if(s<=cursor->str && cursor->str<t){
				t = cursor->str;
				thisstr = cursor;
			}
		if(t==strchars+NSTRCHARS)
			break;
		thisstr->str=s;
		do;while(*s++ = *t++);
	}
	t=strstart;
	strstart=s;
	while(t!=strfree)
		*s++ = *t++;
	strfree=s;
	if(s==strchars+NSTRCHARS){
		strfree=strstart;
		error('Z');
	}
}
