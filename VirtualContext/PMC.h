#ifndef PMC_H
#define PMC_H

/**
 * Типы данных возможные для хранения в регистрах PMC
 */
typedef enum _EParrotMagicCookieType
{
	EPMCT_INTEGER,
	EPMCT_FLOAT,
	EPMCT_STRING,
	EPMCT_HASHTABLE,

	EPMCT_UNINITIALIZED
} EParrotMagicCookieType, *PEParrotMagicCookieType;



typedef struct _SParrotMagicCookie
{
	EParrotMagicCookieType	eType;
	
	union 
	{
		PSSkipList	HashTable;
		INT			Integer;
		FLOAT		Float;
		PCHAR		String;
	} uData;
} SParrotMagicCookie, *PSParrotMagicCookie;


VOID
PmcInitialize(
	PSParrotMagicCookie psPmc
);

BOOL
PmcNew(
	PSParrotMagicCookie		psPmc,
	EParrotMagicCookieType	ePmcType
);

#endif