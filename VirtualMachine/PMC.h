#ifndef PMC_H
#define PMC_H

#include "global.h"

/**
 * “ипы данных возможные дл€ хранени€ в регистрах PMC
 */
typedef enum _EParrotMagicCookieType
{
	EPMCT_INTEGER,
	EPMCT_FLOAT,
	EPMCT_STRING,
	EPMCT_HASHTABLE,
	EPMCT_LIST
} EParrotMagicCookieType, *PEParrotMagicCookieType;



typedef struct _SParrotMagicCookie
{
	EParrotMagicCookieType	eType;
	
	PVOID					pData;
} SParrotMagicCookie, *PSParrotMagicCookie;

#endif