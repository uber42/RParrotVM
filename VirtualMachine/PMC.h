#ifndef PMC_H
#define PMC_H

#include "global.h"

/**
 * “ипы данных возможные дл€ хранени€ в регистрах PMC
 */
typedef enum _EParrotMagicCookieType
{
	EPMCT_Integer,
	EPMCT_Float,
	EPMCT_String,
	EPMCT_HashTable,
	EPMCT_List
} EParrotMagicCookieType, *PEParrotMagicCookieType;

typedef struct _SParrotMagicCookie
{
	EParrotMagicCookieType	eType;
	
	pvoid					pData;
} SParrotMagicCookie, *PSParrotMagicCookie;

#endif