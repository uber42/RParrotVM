#include "global.h"

VOID
PmcInitialize(
	PSParrotMagicCookie psPmc
)
{
	psPmc->eType = EPMCT_UNINITIALIZED;
}