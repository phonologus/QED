#include "qed.h"

extern int noaddr;

void
setdot(void)
{
	if (addr2 == 0)
		addr1 = addr2 = dot;
	if (addr1 > addr2)
		error('$');
}

void
setall(void)
{
	if (noaddr) {
		addr1 = zero+1;
		addr2 = dol;
		if (dol==zero)
			addr1 = zero;
	}
	setdot();
}

void
setnoaddr(void)
{
	extern noaddr;
	if (noaddr == FALSE)
		error('$');
}

void
nonzero(void)
{
	if (addr1<=zero || addr2>dol)
		error('$');
}
