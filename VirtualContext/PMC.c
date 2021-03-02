#include "global.h"


VOID
PmcInitialize(
	PSParrotMagicCookie psPmc
)
{
	psPmc->eType = EPMCT_UNINITIALIZED;
}

BOOL
PmcNew(
	PSParrotMagicCookie		psPmc,
	EParrotMagicCookieType	ePmcType
)
{
	return TRUE;
}