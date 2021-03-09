#include "global.h"

typedef int FSkipListComp(
	PVOID pFirst,
	PVOID pSecond
);

static
INT
HashTableComparator(
	PVOID pFirst,
	PVOID pSecond
)
{
	return strcmp((PCHAR)pFirst, (PCHAR)pSecond);
}

static
VOID
HashTableNodeEraser(
	PVOID pNode
)
{
	PSSkipListNode psSkipListNode = pNode;
	free(psSkipListNode->pValue);
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
	EPmcType				ePmcType
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
		break;
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

EOperandTypes
PmcRecognizeOperand(
	PBYTE			pPmcRegister,
	PBYTE			pTargetMemory
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	memcpy(pTargetMemory, &psPmc->uData, sizeof(UPmcData));
	switch (psPmc->eType)
	{
	case EPMCT_INTEGER:
		return EOT_NUMBER;
	case EPMCT_STRING:
		return EOT_STRING;
	case EPMCT_FLOAT:
		return EOT_FLOAT;
	case EPMCT_HASHTABLE:
		return EOT_SPECIAL;
	default:
		return FALSE;
	}
}

BOOL
PmcHashTableInsert(
	PBYTE			pPmcRegister,
	PCHAR			psKey,
	EOperandTypes	eInsertType,
	PBYTE			pElemMemory
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	if (psPmc->eType != EPMCT_HASHTABLE)
	{
		return FALSE;
	}

	/** Нужен внутренний аллокатор на куче виртуальной машины (Пока что так) */
	PSParrotMagicCookie psNode = malloc(sizeof(SParrotMagicCookie));
	if (!psNode)
	{
		return FALSE;
	}

	UPmcData uData = { 0 };
	memcpy(&uData, pElemMemory, sizeof(UPmcData));

	switch (eInsertType)
	{
	case EOT_FLOAT:
		psNode->eType = EPMCT_FLOAT;
		memcpy(&psNode->uData.Float, &uData.Float, sizeof(FLOAT));
		break;
	case EOT_NUMBER:
		psNode->eType = EPMCT_INTEGER;
		memcpy(&psNode->uData.Integer, &uData.Integer, sizeof(INT));
		break;
	case EOT_STRING:
		psNode->eType = EPMCT_STRING;
		strcpy(psNode->uData.String, uData.String);
		break;
	case EOT_SPECIAL:
		psNode->eType = EPMCT_HASHTABLE;
		psNode->uData.HashTable = CreateSkipList(
			HashTableComparator,
			HashTableNodeEraser,
			HashTableSwapper);
		if (!psNode->uData.HashTable)
		{
			return FALSE;
		}
		SkipListClone(
			psNode->uData.HashTable, uData.HashTable, sizeof(SParrotMagicCookie));
		break;
	default:
		break;
	}

	PSSkipListNode psAdded = SkipListSet(psPmc->uData.HashTable, psKey, psNode);
	if (!psAdded)
	{
		free(psNode);
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcHashTableFind(
	PBYTE			pPmcRegister,
	PCHAR			psKey,
	ERegisterTypes	eInsertType,
	PBYTE			pElemMemory,
	PBOOL			pFound
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	if (psPmc->eType != EPMCT_HASHTABLE)
	{
		return FALSE;
	}

	*pFound = TRUE;
	PSSkipListNode psFound = SkipListFind(psPmc->uData.HashTable, psKey);
	if (!psFound)
	{
		*pFound = FALSE;
		return TRUE;
	}

	PSParrotMagicCookie psPmcFound = psFound->pValue;
	switch (psPmcFound->eType)
	{
	case EPMCT_INTEGER:
		memcpy(pElemMemory, &psPmcFound->uData.Integer, sizeof(INT));
		break;
	case EPMCT_FLOAT:
		memcpy(pElemMemory, &psPmcFound->uData.Float, sizeof(FLOAT));
		break;
	case EPMCT_STRING:
		strcpy(pElemMemory, psPmcFound->uData.String);
		break;
	case EPMCT_HASHTABLE:
	{
		PSParrotMagicCookie psPmc = pElemMemory;
		psPmc->uData.HashTable = CreateSkipList(
			HashTableComparator,
			HashTableNodeEraser,
			HashTableSwapper);
		if (!psPmc->uData.HashTable)
		{
			return FALSE;
		}
		memcpy(psPmc->uData.HashTable, psPmcFound->uData.HashTable, sizeof(SSkipList));
		psPmc->eType = EPMCT_HASHTABLE;
		break;
	}
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL
PmcHashTableRemove(
	PBYTE			pPmcRegister,
	PCHAR			psKey,
	PBOOL			pRemoved
)
{
	PSParrotMagicCookie psPmc = pPmcRegister;
	if (psPmc->eType != EPMCT_HASHTABLE)
	{
		return FALSE;
	}

	*pRemoved = SkipListRemove(psPmc->uData.HashTable, psKey);

	return TRUE;
}