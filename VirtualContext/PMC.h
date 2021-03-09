#ifndef PMC_H
#define PMC_H

/**
 * Типы данных возможные для хранения в регистрах PMC
 */
typedef enum _EPmcType
{
	EPMCT_INTEGER = 1 << 9,
	EPMCT_FLOAT = 1 << 10,
	EPMCT_STRING = 1 << 11,
	EPMCT_HASHTABLE = 1 << 13,

	EPMCT_UNINITIALIZED = 0xFFFF
} EPmcType, *PEPmcType;

typedef union _UPmcData
{
	PSSkipList	HashTable;
	INT			Integer;
	FLOAT		Float;
	CHAR		String[STRING_MAX_LENGTH];
} UPmcData;


typedef struct _SParrotMagicCookie
{
	EPmcType		eType;
	
	UPmcData		uData;
} SParrotMagicCookie, *PSParrotMagicCookie;


VOID
PmcInitialize(
	PSParrotMagicCookie psPmc
);

BOOL
PmcNew(
	PSParrotMagicCookie		psPmc,
	EPmcType				ePmcType
);

BOOL
PmcGetFloat(
	PBYTE	pDestMemory,
	PBYTE	pPmcRegister
);

BOOL
PmcSetFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
);

BOOL
PmcSetInteger(
	PBYTE	pPmcRegister,
	INT		dwValue
);

BOOL
PmcSetString(
	PBYTE	pPmcRegister,
	PCHAR	szString
);

BOOL
PmcIncrement(
	PBYTE	pPmcRegister
);

BOOL
PmcDecrement(
	PBYTE	pPmcRegister
);

BOOL
PmcAddInteger(
	PBYTE	pPmcRegister,
	INT		nValue
);

BOOL
PmcAddFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
);

BOOL
PmcSubInteger(
	PBYTE	pPmcRegister,
	INT		nValue
);

BOOL
PmcSubFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
);

BOOL
PmcMulInteger(
	PBYTE	pPmcRegister,
	INT		nValue
);

BOOL
PmcMulFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
);

BOOL
PmcDivInteger(
	PBYTE	pPmcRegister,
	INT		nValue
);

BOOL
PmcDivFloat(
	PBYTE	pPmcRegister,
	FLOAT	fValue
);

BOOL
PmcGetNativeNumber(
	PBYTE			pPmcRegister,
	PEOperandTypes	peOperand,
	DWORD			dwNumber
);

BOOL
PmcPrint(
	PBYTE	pPmcRegister
);

PCHAR
PmcGetString(
	PBYTE	pPmcRegister
);

EPmcType
PmcGetType(
	PBYTE	pPmcRegister
);

EOperandTypes
PmcRecognizeOperand(
	PBYTE			pPmcRegister,
	PBYTE			pTargetMemory
);

BOOL
PmcHashTableInsert(
	PBYTE			pPmcRegister,
	PCHAR			psKey,
	EOperandTypes	eInsertType,
	PBYTE			pElemMemory
);

BOOL
PmcHashTableFind(
	PBYTE			pPmcRegister,
	PCHAR			psKey,
	ERegisterTypes	eInsertType,
	PBYTE			pElemMemory
);


#endif