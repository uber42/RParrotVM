#include "global.h"


typedef int FSkipListComp(
	PVOID pFirst,
	PVOID pSecond
);

static
BOOL
HashTableComparator(
	PVOID pFirst,
	PVOID pSecond
)
{
	return strcmp((PCHAR)pFirst, (PCHAR)pSecond) >= 0;
}

static
VOID
HashTableNodeEraser(
	PVOID pNode
)
{
	UNREFERENCED_PARAMETER(pNode);
}

static
VOID
HashTableSwapper(
	PVOID*	pDest,
	PVOID	pSrc
)
{
	*pDest = pSrc;
}

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
	EPmcType	ePmcType
)
{
	switch (ePmcType)
	{
	case EPMCT_INTEGER:
		psPmc->uData.Integer = 0;
		break;
	case EPMCT_FLOAT:
		psPmc->uData.Float = 0.f;
		break;
	case EPMCT_STRING:
		memset(psPmc->uData.String, 0, STRING_MAX_LENGTH);
		break;
	case EPMCT_HASHTABLE:
		psPmc->uData.HashTable = CreateSkipList(
			HashTableComparator,
			HashTableNodeEraser,
			HashTableSwapper);
		if (!psPmc->uData.HashTable)
		{
			return FALSE;
		}
	default:
		return FALSE;
	}

	psPmc->eType = ePmcType;
	return TRUE;
}

BOOL
PmcGetFloat(
	PBYTE	pDestMemory,
	PBYTE	pPmcRegister
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	if (psPmc->eType != EPMCT_FLOAT)
	{
		return FALSE;
	}

	memcpy(pDestMemory, &psPmc->uData.Float, sizeof(FLOAT));
	return TRUE;
}

BOOL
PmcSetFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	if (psPmc->eType != EPMCT_FLOAT)
	{
		return FALSE;
	}

	psPmc->uData.Float = fValue;
	return TRUE;
}

BOOL
PmcSetInteger(
	PBYTE	pPmcRegister,
	INT		dwValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	if (psPmc->eType != EPMCT_INTEGER)
	{
		return FALSE;
	}

	psPmc->uData.Integer = dwValue;
	return TRUE;
}

BOOL
PmcSetString(
	PBYTE	pPmcRegister,
	PCHAR	szString
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	if (psPmc->eType != EPMCT_STRING)
	{
		return FALSE;
	}

	strcpy(psPmc->uData.String, szString);
	return TRUE;
}

BOOL
PmcIncrement(
	PBYTE	pPmcRegister
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float++;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer++;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcDecrement(
	PBYTE	pPmcRegister
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float--;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer--;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcAddInteger(
	PBYTE	pPmcRegister,
	INT		nValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float += nValue;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer += nValue;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcAddFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float += fValue;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer += fValue;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcSubInteger(
	PBYTE	pPmcRegister,
	INT		nValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float -= nValue;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer -= nValue;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcSubFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float -= fValue;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer -= fValue;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcMulInteger(
	PBYTE	pPmcRegister,
	INT		nValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float *= nValue;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer *= nValue;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcMulFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float *= fValue;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer *= fValue;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcDivInteger(
	PBYTE	pPmcRegister,
	INT		nValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float /= nValue;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer /= nValue;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcDivFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		psPmc->uData.Float /= fValue;
		break;
	case EPMCT_INTEGER:
		psPmc->uData.Integer /= fValue;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcGetNativeNumber(
	PBYTE			pPmcRegister,
	PEOperandTypes	peOperand,
	DWORD			dwNumber
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		memcpy(&dwNumber, &psPmc->uData.Float, sizeof(DWORD));
		*peOperand = EOT_FLOAT;
		break;
	case EPMCT_INTEGER:
		memcpy(&dwNumber, &psPmc->uData.Integer, sizeof(DWORD));
		*peOperand = EOT_NUMBER;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcPrint(
	PBYTE	pPmcRegister
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_FLOAT:
		printf("%f", psPmc->uData.Float);
		break;
	case EPMCT_INTEGER:
		printf("%d", psPmc->uData.Integer);
		break;
	case EPMCT_STRING:
		printf("%s", psPmc->uData.String);
		break;
	case EPMCT_HASHTABLE:
		// HashTablePrint()
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

PCHAR
PmcGetString(
	PBYTE	pPmcRegister
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	switch (psPmc->eType)
	{
	case EPMCT_STRING:
		return psPmc->uData.String;
	default:
		return NULL;
	}
}

EPmcType
PmcGetType(
	PBYTE	pPmcRegister
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	return psPmc->eType;
}