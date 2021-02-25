#ifndef PMC_H
#define PMC_H

/**
 * “ипы данных возможные дл€ хранени€ в регистрах PMC
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
	
	PVOID					pData;
} SParrotMagicCookie, *PSParrotMagicCookie;


VOID
PmcInitialize(
	PSParrotMagicCookie psPmc
);

#endif