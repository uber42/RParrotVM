#ifndef PMC_H
#define PMC_H

/**
 * ���� ������ ��������� ��� �������� � ��������� PMC
 */
typedef enum _EParrotMagicCookieType
{
	EPMCT_INTEGER,
	EPMCT_FLOAT,
	EPMCT_STRING,
	EPMCT_HASHTABLE
} EParrotMagicCookieType, *PEParrotMagicCookieType;



typedef struct _SParrotMagicCookie
{
	EParrotMagicCookieType	eType;
	
	PVOID					pData;
} SParrotMagicCookie, *PSParrotMagicCookie;

#endif