#include "global.h"

#define OPERAND_CUT_MASK		0x78000000
#define NUMBER_CUT_MASK			0x1FFFFFFF

#define NO_SUCCESS_EXECUTION	((DWORD)-1)

typedef enum _EOperandTypes
{
	EOT_STRING,
	EOT_NUMBER,
	EOT_REGISTER,
	EOT_VIRTUAL_MEMORY,
	EOT_SPECIAL
} EOperandTypes, *PEOperandTypes;

typedef enum _ERegisterTypes
{
	ERT_I = 1 << 0,
	ERT_N = 1 << 1,
	ERT_S = 1 << 2,
	ERT_P = 1 << 3
} ERegisterTypes, *PERegisterTypes;

static
EOperandTypes
RecognizeOperand(
	DWORD	dwOperand
)
{
	DWORD dwCutedOperand = dwOperand & OPERAND_CUT_MASK;
	switch (dwCutedOperand)
	{
	case BYTECODE_STRING_LITERAL_FLAG:
		return EOT_STRING;
	case BYTECODE_NUMBER_LITERAL_FLAG:
		return EOT_NUMBER;
	case BYTECODE_VIRTUAL_MEMORY_FLAG:
		return EOT_VIRTUAL_MEMORY;
	case BYTECODE_REGISTER_FLAG:
		return EOT_REGISTER;
	default:
		return EOT_SPECIAL;
	}
}

static
ERegisterTypes
RecognizeRegister(
	DWORD				dwOperand,
	PSVirtualProcessor  psVirtualProcessor,
	PBYTE*				pbTargetMemory,
	PDWORD				pdwSize
)
{
	DWORD dwRegisterNumber = (dwOperand >> 25) & 0x3;
	DWORD dwRegisterType = dwOperand & 0xF;

	switch (dwRegisterType)
	{
	case ERT_I:
		*pbTargetMemory = &psVirtualProcessor->I[dwRegisterNumber];
		*pdwSize = sizeof(INT);
		return dwRegisterType;
	case ERT_N:
		*pbTargetMemory = &psVirtualProcessor->N[dwRegisterNumber];
		*pdwSize = sizeof(FLOAT);
		return dwRegisterType;
	case ERT_S:
		*pbTargetMemory = &psVirtualProcessor->S[dwRegisterNumber];
		*pdwSize = STRING_MAX_LENGTH * sizeof(CHAR);
		return dwRegisterType;
	case ERT_P:
		*pbTargetMemory = &psVirtualProcessor->P[dwRegisterNumber];
		*pdwSize = sizeof(SParrotMagicCookie);
		return dwRegisterType;
	default:
		return 0;
	}
}

static
PBYTE
GetVirtualMemory(
	DWORD				dwOperand,
	PBYTE				psHeap
)
{
	DWORD dwOffset = dwOperand & 0xFFFF;
	return psHeap + dwOffset;
}

static
PCHAR
GetStringLitral(
	DWORD				dwOperand,
	PBYTE				psIndexTable
)
{
	DWORD dwNumber = dwOperand & 0xFFFF;
	DWORD dwOffset = 0;

	memcpy(&dwOffset, psIndexTable + dwNumber * sizeof(DWORD), sizeof(DWORD));
	return psIndexTable + dwOffset - sizeof(SFrozenFileHeader);
}

static
PCHAR
ExtractString(
	DWORD				dwOperand,
	PSVirtualProcessor  psVirtualProcessor,
	PBYTE				pIndexTable
)
{
	EOperandTypes eOperandType = RecognizeOperand(dwOperand);
	switch (eOperandType)
	{
	case EOT_REGISTER:
	{
		DWORD dwSize = 0;
		PBYTE pTargetMemory = NULL;
		ERegisterTypes eRegisterType = RecognizeRegister(
			dwOperand,
			psVirtualProcessor,
			&pTargetMemory,
			&dwSize);
		if (eRegisterType != ERT_S)
		{
			return NULL;
		}

		return pTargetMemory;
	}
	case EOT_STRING:
		return GetStringLitral(
			dwOperand, pIndexTable);
	default:
		return NULL;
	}
}

static
INT
GetNumberLiteral(
	DWORD	dwOperand
)
{
	return dwOperand & NUMBER_CUT_MASK;
}

static
BOOL
PmcAbleSetRegister(
	PSParrotMagicCookie	psPmc,
	ERegisterTypes		eRegister
)
{
	switch (psPmc->eType)
	{
	case EPMCT_INTEGER:
	case EPMCT_FLOAT:
		return eRegister == ERT_I || eRegister == ERT_N;
	case EPMCT_STRING:
		return eRegister == ERT_S;
	case EPMCT_HASHTABLE:
		return FALSE;
	case EPMCT_UNINITIALIZED:
		return FALSE;
	default:
		return FALSE;
	}
}

static
BOOL
IsRegistersCompatibile(
	ERegisterTypes	eDestRegister,
	ERegisterTypes	eSrcRegister,
	PBYTE			pSrcMem
)
{
	switch (eDestRegister)
	{
	case ERT_I:
	case ERT_N:
		return (eSrcRegister & (ERT_I | ERT_N)) == eSrcRegister || 
			(eSrcRegister == ERT_P && PmcAbleSetRegister(pSrcMem, eDestRegister));
	case ERT_S:
		return (eSrcRegister & ERT_S) == eSrcRegister ||
			(eSrcRegister == ERT_P && PmcAbleSetRegister(pSrcMem, eDestRegister));
	case ERT_P:
		return TRUE;
	default:
		return FALSE;
	}
}

static
BOOL
RegisterAdd(
	PBYTE	pTargetMemery,
	INT		dwAddition
)
{
	INT dwCurrentValue = 0;
	memcpy(&dwCurrentValue, pTargetMemery, sizeof(INT));

	dwCurrentValue += dwAddition;
	memcpy(pTargetMemery, &dwCurrentValue, sizeof(INT));

	return TRUE;
}

static
BOOL
RegisterMul(
	PBYTE	pTargetMemery,
	INT		dwFactor
)
{
	INT dwCurrentValue = 0;
	memcpy(&dwCurrentValue, pTargetMemery, sizeof(INT));

	dwCurrentValue *= dwFactor;
	memcpy(pTargetMemery, &dwCurrentValue, sizeof(INT));

	return TRUE;
}

static
BOOL
RegisterDiv(
	PBYTE	pTargetMemery,
	INT		dwNumber
)
{
	INT dwCurrentValue = 0;
	memcpy(&dwCurrentValue, pTargetMemery, sizeof(INT));

	dwCurrentValue /= dwNumber;
	memcpy(pTargetMemery, &dwCurrentValue, sizeof(INT));

	return TRUE;
}

static
BOOL
GetNativeNumber(
	PSVirtualProcessor	psVirtualProcessor,
	EOperandTypes		eOpType,
	DWORD				dwOperand,
	PDWORD				pdwNumber
)
{
	switch (eOpType)
	{
	case EOT_NUMBER:
	{
		DWORD dwLiteral = GetNumberLiteral(dwOperand);
		memcpy(pdwNumber, &dwLiteral, sizeof(DWORD));
		return TRUE;
	}
	case EOT_REGISTER:
	{
		DWORD dwSize = 0;
		PBYTE pTargetMemory = NULL;
		ERegisterTypes eSrcRegister = RecognizeRegister(
			dwOperand,
			psVirtualProcessor,
			&pTargetMemory,
			&dwSize);
		if (eSrcRegister != ERT_I)
		{
			return FALSE;
		}

		memcpy(pdwNumber, pTargetMemory, sizeof(DWORD));
		return TRUE;
	}
	default:
		return FALSE;
	}
}

BOOL
PasmSet(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				psHeap,
	PBYTE				psIndexTable,
	PBYTE				pCurrentInstruction
)
{
	DWORD pdwOperands[2];
	memcpy(pdwOperands, pCurrentInstruction, sizeof(pdwOperands));
	EOperandTypes peTypes[2] = 
	{ 
		RecognizeOperand(pdwOperands[0]), 
		RecognizeOperand(pdwOperands[1]) 
	};

	PBYTE pTargetMemory = NULL;
	if (peTypes[0] == EOT_REGISTER)
	{
		DWORD dwDestSize = 0;
		ERegisterTypes eDestRegister = RecognizeRegister(
			pdwOperands[0],
			psVirtualProcessor,
			&pTargetMemory,
			&dwDestSize);
		switch(peTypes[1])
		{
		case EOT_REGISTER:
		{
			DWORD dwSrcSize = 0;
			PBYTE pSrcMemory = NULL;
			ERegisterTypes eSrcRegister = RecognizeRegister(
				pdwOperands[1],
				psVirtualProcessor,
				&pSrcMemory,
				&dwSrcSize);
			BOOL bCompatibile = IsRegistersCompatibile(
				eDestRegister,
				eSrcRegister,
				pSrcMemory
			);
			if (!bCompatibile)
			{
				return FALSE;
			}

			assert(dwSrcSize == dwDestSize);
			memcpy(pTargetMemory, pSrcMemory, dwSrcSize);

			return TRUE;
		}
		case EOT_NUMBER:
		{
			if (eDestRegister == ERT_S)
			{
				return FALSE;
			}

			if (eDestRegister == ERT_P)
			{
				// PmcCopyMem();
				return TRUE; 
			}
			else
			{
				DWORD dwLiteral = GetNumberLiteral(pdwOperands[1]);
				memcpy(pTargetMemory, &dwLiteral, sizeof(DWORD));
				return TRUE;
			}
		}
		case EOT_VIRTUAL_MEMORY:
		{
			if (eDestRegister == ERT_P)
			{
				return FALSE;
			}

			PBYTE pSrcMem = GetVirtualMemory(
				pdwOperands[1],
				psHeap);
			memcpy(pTargetMemory, pSrcMem, dwDestSize);
			return TRUE;
		}
		case EOT_STRING:
		{
			if (eDestRegister == ERT_S)
			{
				PCHAR pString = GetStringLitral(
					pdwOperands[1], 
					psIndexTable);
				SIZE_T nLen = strlen(pString);
				memcpy(pTargetMemory, pString, nLen + 1);
				return TRUE;
			}
			else if (eDestRegister == ERT_P)
			{
				// PmcCopyMem();
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		default:
		{
			return FALSE;
		}
		}
	}
	else if(peTypes[0] == EOT_VIRTUAL_MEMORY)
	{
		pTargetMemory = GetVirtualMemory(
			pdwOperands[0],
			psHeap);
		switch (peTypes[1])
		{
		case EOT_REGISTER:
		{
			DWORD dwSrcSize = 0;
			PBYTE pSrcMemory = NULL;
			ERegisterTypes eSrcRegister = RecognizeRegister(
				pdwOperands[1],
				psVirtualProcessor,
				&pSrcMemory,
				&dwSrcSize);
			if (eSrcRegister == ERT_P)
			{
				return FALSE;
			}

			memcpy(pTargetMemory, pSrcMemory, dwSrcSize);
			return TRUE;
		}
		case EOT_NUMBER:
		{
			DWORD dwLiteral = GetNumberLiteral(pdwOperands[1]);
			memcpy(pTargetMemory, &dwLiteral, sizeof(DWORD));
			return TRUE;
		}
		case EOT_STRING:
		{
			PCHAR pString = GetStringLitral(
				pdwOperands[1],
				psIndexTable);
			SIZE_T nLen = strlen(pString);
			memcpy(pTargetMemory, pString, nLen + 1);
			return TRUE;
		}
		default:
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL
PasmInc(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand;
	memcpy(&dwOperand, pCurrentInstruction, sizeof(DWORD));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand, psVirtualProcessor,
		&pTargetMemory, &dwSize);
	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		return RegisterAdd(
			pTargetMemory, 1);
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmDec(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand;
	memcpy(&dwOperand, pCurrentInstruction, sizeof(DWORD));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand, psVirtualProcessor,
		&pTargetMemory, &dwSize);
	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		return RegisterAdd(
			pTargetMemory, -1);
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmAdd3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[3];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand[0], psVirtualProcessor,
		&pTargetMemory, &dwSize);

	INT dwNumber = 0;
	EOperandTypes eOperandType[2] =
	{
		RecognizeOperand(dwOperand[1]),
		RecognizeOperand(dwOperand[2])
	};

	DWORD dwFirstNumber = 0;
	DWORD dwSecondNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[0],
		dwOperand[1],
		&dwFirstNumber);
	if (!bResult)
	{
		return FALSE;
	}
	
	bResult = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[1],
		dwOperand[2],
		&dwSecondNumber);
	if (!bResult)
	{
		return FALSE;
	}

	DWORD dwResult = dwFirstNumber + dwSecondNumber;
	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		memcpy(pTargetMemory, &dwResult, sizeof(DWORD));
		return TRUE;
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmSub3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[3];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand[0], psVirtualProcessor,
		&pTargetMemory, &dwSize);

	INT dwNumber = 0;
	EOperandTypes eOperandType[2] =
	{
		RecognizeOperand(dwOperand[1]),
		RecognizeOperand(dwOperand[2])
	};

	DWORD dwFirstNumber = 0;
	DWORD dwSecondNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[0],
		dwOperand[1],
		&dwFirstNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[1],
		dwOperand[2],
		&dwSecondNumber);
	if (!bResult)
	{
		return FALSE;
	}

	DWORD dwResult = dwFirstNumber - dwSecondNumber;
	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		memcpy(pTargetMemory, &dwResult, sizeof(DWORD));
		return TRUE;
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmMul3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[3];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand[0], psVirtualProcessor,
		&pTargetMemory, &dwSize);

	INT dwNumber = 0;
	EOperandTypes eOperandType[2] =
	{
		RecognizeOperand(dwOperand[1]),
		RecognizeOperand(dwOperand[2])
	};

	DWORD dwFirstNumber = 0;
	DWORD dwSecondNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[0],
		dwOperand[1],
		&dwFirstNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[1],
		dwOperand[2],
		&dwSecondNumber);
	if (!bResult)
	{
		return FALSE;
	}

	DWORD dwResult = dwFirstNumber * dwSecondNumber;
	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		memcpy(pTargetMemory, &dwResult, sizeof(DWORD));
		return TRUE;
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmDiv3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[3];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand[0], psVirtualProcessor,
		&pTargetMemory, &dwSize);

	INT dwNumber = 0;
	EOperandTypes eOperandType[2] =
	{
		RecognizeOperand(dwOperand[1]),
		RecognizeOperand(dwOperand[2])
	};

	DWORD dwFirstNumber = 0;
	DWORD dwSecondNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[0],
		dwOperand[1],
		&dwFirstNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[1],
		dwOperand[2],
		&dwSecondNumber);
	if (!bResult)
	{
		return FALSE;
	}

	DWORD dwResult = dwFirstNumber / dwSecondNumber;
	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		memcpy(pTargetMemory, &dwResult, sizeof(DWORD));
		return TRUE;
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmAdd2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[2];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand[0], psVirtualProcessor,
		&pTargetMemory, &dwSize);

	INT dwNumber = 0;
	EOperandTypes eOperandType = RecognizeOperand(dwOperand[1]);
	switch (eOperandType)
	{
	case EOT_NUMBER:
		dwNumber = GetNumberLiteral(dwOperand[1]);
		break;
	case EOT_REGISTER:
	{
		PBYTE pDestMemory = NULL;
		ERegisterTypes eRegisterType = RecognizeRegister(
			dwOperand[1], psVirtualProcessor,
			&pDestMemory, &dwSize);
		switch (eRegisterType)
		{
		case ERT_I:
		case ERT_N:
			memcpy(&dwNumber, pDestMemory, dwSize);
			break;
		default:
			return FALSE;
		}
		break;
	}
	default:
		return FALSE;
	}	

	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		return RegisterAdd(
			pTargetMemory, dwNumber);
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmSub2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[2];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand[0], psVirtualProcessor,
		&pTargetMemory, &dwSize);

	INT dwNumber = 0;
	EOperandTypes eOperandType = RecognizeOperand(dwOperand[1]);
	switch (eOperandType)
	{
	case EOT_NUMBER:
		dwNumber = GetNumberLiteral(dwOperand[1]);
		break;
	case EOT_REGISTER:
	{
		PBYTE pDestMemory = NULL;
		ERegisterTypes eRegisterType = RecognizeRegister(
			dwOperand[1], psVirtualProcessor,
			&pDestMemory, &dwSize);
		switch (eRegisterType)
		{
		case ERT_I:
		case ERT_N:
			memcpy(&dwNumber, pDestMemory, dwSize);
			break;
		default:
			return FALSE;
		}
		break;
	}
	default:
		return FALSE;
	}

	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		return RegisterAdd(
			pTargetMemory, -dwNumber);
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmMul2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[2];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand[0], psVirtualProcessor,
		&pTargetMemory, &dwSize);

	INT dwNumber = 0;
	EOperandTypes eOperandType = RecognizeOperand(dwOperand[1]);
	switch (eOperandType)
	{
	case EOT_NUMBER:
		dwNumber = GetNumberLiteral(dwOperand[1]);
		break;
	case EOT_REGISTER:
	{
		PBYTE pDestMemory = NULL;
		ERegisterTypes eRegisterType = RecognizeRegister(
			dwOperand[1], psVirtualProcessor,
			&pDestMemory, &dwSize);
		switch (eRegisterType)
		{
		case ERT_I:
		case ERT_N:
			memcpy(&dwNumber, pDestMemory, dwSize);
			break;
		default:
			return FALSE;
		}
		break;
	}
	default:
		return FALSE;
	}

	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		return RegisterMul(
			pTargetMemory, dwNumber);
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmDiv2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[2];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand[0], psVirtualProcessor,
		&pTargetMemory, &dwSize);

	INT dwNumber = 0;
	EOperandTypes eOperandType = RecognizeOperand(dwOperand[1]);
	switch (eOperandType)
	{
	case EOT_NUMBER:
		dwNumber = GetNumberLiteral(dwOperand[1]);
		break;
	case EOT_REGISTER:
	{
		PBYTE pDestMemory = NULL;
		ERegisterTypes eRegisterType = RecognizeRegister(
			dwOperand[1], psVirtualProcessor,
			&pDestMemory, &dwSize);
		switch (eRegisterType)
		{
		case ERT_I:
		case ERT_N:
			memcpy(&dwNumber, pDestMemory, dwSize);
			break;
		default:
			return FALSE;
		}
		break;
	}
	default:
		return FALSE;
	}

	switch (eRegisterType)
	{
	case ERT_I:
	case ERT_N:
		return RegisterDiv(
			pTargetMemory, dwNumber);
	case ERT_P:
		return TRUE; // PmcIncrement(pTargetMemory)
	default:
		return FALSE;
	}
}

BOOL
PasmPrint(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				psIndexTable
)
{
	DWORD dwOperand;
	memcpy(&dwOperand, pCurrentInstruction, sizeof(DWORD));

	EOperandTypes eOperandType = RecognizeOperand(dwOperand);
	switch (eOperandType)
	{
	case EOT_NUMBER:
	{
		DWORD dwNumber = GetNumberLiteral(dwOperand);
		printf("%d", dwNumber);

		return TRUE;
	}
	case EOT_STRING:
	{
		PCHAR pString = GetStringLitral(
			dwOperand,
			psIndexTable);
		printf("%s", pString);

		return TRUE;
	}
	case EOT_REGISTER:
	{
		DWORD dwSrcSize = 0;
		PBYTE pSrcMemory = NULL;
		ERegisterTypes eSrcRegister = RecognizeRegister(
			dwOperand,
			psVirtualProcessor,
			&pSrcMemory,
			&dwSrcSize);
		switch (eSrcRegister)
		{
		case ERT_I:
		case ERT_N:
		{
			DWORD dwNumber = 0;
			memcpy(&dwNumber, pSrcMemory, dwSrcSize);
			printf("%d", dwNumber);

			return TRUE;
		}
		case ERT_S:
		{
			printf("%s", (PCHAR)pSrcMemory);
			return TRUE;
		}
		case ERT_P:
		{
			return TRUE; // PmcPrint();
		}
		default:
			return FALSE;
		}
	}
	default:
		return FALSE;
	}
}

BOOL
PasmLength(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				psIndexTable
)
{
	DWORD dwOperand[2];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	BOOL bResult = RecognizeRegister(
		dwOperand[0],
		psVirtualProcessor,
		&pTargetMemory,
		&dwSize);
	if (!bResult)
	{
		return FALSE;
	}

	PCHAR pString = GetStringLitral(
		dwOperand[1],
		psIndexTable);
	if (!bResult)
	{
		return FALSE;
	}

	SIZE_T nLength = strlen(pString);
	memcpy(pTargetMemory, nLength, dwSize);

	return TRUE;
}

BOOL
PasmConcat(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				psIndexTable
)
{
	DWORD dwOperand[2];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	BOOL bResult = RecognizeRegister(
		dwOperand[0],
		psVirtualProcessor,
		&pTargetMemory,
		&dwSize);
	if (!bResult)
	{
		return FALSE;
	}

	PCHAR pDestString = GetStringLitral(
		dwOperand[1],
		psIndexTable);
	if (!bResult)
	{
		return FALSE;
	}

	CHAR szCurrentString[STRING_MAX_LENGTH];
	strcpy(szCurrentString, pTargetMemory);
	strcat(szCurrentString, pDestString);
	memcpy(pTargetMemory, szCurrentString, sizeof(szCurrentString));

	return TRUE;
}

BOOL
PasmSubstr(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				psIndexTable
)
{
	DWORD dwOperand[4];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	BOOL bResult = RecognizeRegister(
		dwOperand[0],
		psVirtualProcessor,
		&pTargetMemory,
		&dwSize);
	if (!bResult)
	{
		return FALSE;
	}

	PCHAR pString = ExtractString(
		dwOperand[1],
		psVirtualProcessor,
		psIndexTable);
	if (!pString)
	{
		return FALSE;
	}

	EOperandTypes peTypes[2] =
	{
		RecognizeOperand(dwOperand[2]),
		RecognizeOperand(dwOperand[3])
	};

	DWORD dwLowerBound = 0;
	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[0],
		dwOperand[2],
		&dwLowerBound);
	if (!bResult)
	{
		return FALSE;
	}

	DWORD dwCharCount = 0;
	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[1],
		dwOperand[3],
		&dwCharCount);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwLowerBound + dwCharCount > STRING_MAX_LENGTH)
	{
		return FALSE;
	}

	memcpy(pTargetMemory, pString + dwLowerBound, dwCharCount);

	return TRUE;
}

BOOL
PasmBranch(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwMarker;
	memcpy(&dwMarker, pCurrentInstruction, sizeof(DWORD));

	psVirtualProcessor->IP = dwMarker;
	return TRUE;
}

BOOL
PasmIf2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand;
	DWORD dwMarker;

	memcpy(&dwOperand, pCurrentInstruction, sizeof(DWORD));
	memcpy(&dwMarker, pCurrentInstruction + sizeof(dwOperand), sizeof(DWORD));

	EOperandTypes peType = RecognizeOperand(dwOperand);

	DWORD dwNumber = 0;
	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peType,
		dwOperand,
		&dwNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwNumber)
	{
		psVirtualProcessor->IP = dwMarker;
	}
	else
	{
		psVirtualProcessor->IP += sizeof(dwOperand) + sizeof(dwMarker) + sizeof(BYTE);
	}

	return TRUE;
}

BOOL
PasmIf3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand;
	DWORD dwMarkers[2];

	memcpy(&dwOperand, pCurrentInstruction, sizeof(DWORD));
	memcpy(dwMarkers, pCurrentInstruction + sizeof(dwOperand), sizeof(dwMarkers));

	EOperandTypes peType = RecognizeOperand(dwOperand);

	DWORD dwNumber = 0;
	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peType,
		dwOperand,
		&dwNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwNumber)
	{
		psVirtualProcessor->IP = dwMarkers[0];
	}
	else
	{
		psVirtualProcessor->IP = dwMarkers[1];
	}

	return TRUE;
}

BOOL
PasmGt3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD pdwOperands[2];
	DWORD dwMarker;

	memcpy(pdwOperands, pCurrentInstruction, sizeof(pdwOperands));
	memcpy(&dwMarker, pCurrentInstruction + sizeof(pdwOperands), sizeof(DWORD));

	EOperandTypes peTypes[2] =
	{
		RecognizeOperand(pdwOperands[0]),
		RecognizeOperand(pdwOperands[1])
	};

	DWORD dwLeftNumber = 0;
	DWORD dwRightNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[0],
		pdwOperands[0],
		&dwLeftNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[1],
		pdwOperands[1],
		&dwRightNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwLeftNumber > dwRightNumber)
	{
		psVirtualProcessor->IP = dwMarker;
	}
	else
	{
		psVirtualProcessor->IP += sizeof(pdwOperands) + sizeof(dwMarker) + sizeof(BYTE);
	}

	return TRUE;
}

BOOL
PasmLt3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD pdwOperands[2];
	DWORD dwMarker;

	memcpy(pdwOperands, pCurrentInstruction, sizeof(pdwOperands));
	memcpy(&dwMarker, pCurrentInstruction + sizeof(pdwOperands), sizeof(DWORD));

	EOperandTypes peTypes[2] =
	{
		RecognizeOperand(pdwOperands[0]),
		RecognizeOperand(pdwOperands[1])
	};

	DWORD dwLeftNumber = 0;
	DWORD dwRightNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[0],
		pdwOperands[0],
		&dwLeftNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[1],
		pdwOperands[1],
		&dwRightNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwLeftNumber < dwRightNumber)
	{
		psVirtualProcessor->IP = dwMarker;
	}
	else
	{
		psVirtualProcessor->IP += sizeof(pdwOperands) + sizeof(dwMarker) + sizeof(BYTE);
	}

	return TRUE;
}

BOOL
PasmEq3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD pdwOperands[2];
	DWORD dwMarker;

	memcpy(pdwOperands, pCurrentInstruction, sizeof(pdwOperands));
	memcpy(&dwMarker, pCurrentInstruction + sizeof(pdwOperands), sizeof(DWORD));

	EOperandTypes peTypes[2] =
	{
		RecognizeOperand(pdwOperands[0]),
		RecognizeOperand(pdwOperands[1])
	};

	DWORD dwLeftNumber = 0;
	DWORD dwRightNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[0],
		pdwOperands[0],
		&dwLeftNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[1],
		pdwOperands[1],
		&dwRightNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwLeftNumber == dwRightNumber)
	{
		psVirtualProcessor->IP = dwMarker;
	}
	else
	{
		psVirtualProcessor->IP += sizeof(pdwOperands) + sizeof(dwMarker) + sizeof(BYTE);
	}

	return TRUE;
}

BOOL
PasmNe3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD pdwOperands[2];
	DWORD dwMarker;

	memcpy(pdwOperands, pCurrentInstruction, sizeof(pdwOperands));
	memcpy(&dwMarker, pCurrentInstruction + sizeof(pdwOperands), sizeof(DWORD));

	EOperandTypes peTypes[2] =
	{
		RecognizeOperand(pdwOperands[0]),
		RecognizeOperand(pdwOperands[1])
	};

	DWORD dwLeftNumber = 0;
	DWORD dwRightNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[0],
		pdwOperands[0],
		&dwLeftNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[1],
		pdwOperands[1],
		&dwRightNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwLeftNumber != dwRightNumber)
	{
		psVirtualProcessor->IP = dwMarker;
	}
	else
	{
		psVirtualProcessor->IP += sizeof(pdwOperands) + sizeof(dwMarker) + sizeof(BYTE);
	}

	return TRUE;
}

BOOL
PasmGt4(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD pdwOperands[2];
	DWORD pdwMarkerks[2];

	memcpy(pdwOperands, pCurrentInstruction, sizeof(pdwOperands));
	memcpy(pdwMarkerks, pCurrentInstruction + sizeof(pdwOperands), sizeof(pdwMarkerks));

	EOperandTypes peTypes[2] =
	{
		RecognizeOperand(pdwOperands[0]),
		RecognizeOperand(pdwOperands[1])
	};

	DWORD dwLeftNumber = 0;
	DWORD dwRightNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[0],
		pdwOperands[0],
		&dwLeftNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[1],
		pdwOperands[1],
		&dwRightNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwLeftNumber > dwRightNumber)
	{
		psVirtualProcessor->IP = pdwMarkerks[0];
	}
	else
	{
		psVirtualProcessor->IP = pdwMarkerks[1];
	}

	return TRUE;
}

BOOL
PasmLt4(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD pdwOperands[2];
	DWORD pdwMarkerks[2];

	memcpy(pdwOperands, pCurrentInstruction, sizeof(pdwOperands));
	memcpy(pdwMarkerks, pCurrentInstruction + sizeof(pdwOperands), sizeof(pdwMarkerks));

	EOperandTypes peTypes[2] =
	{
		RecognizeOperand(pdwOperands[0]),
		RecognizeOperand(pdwOperands[1])
	};

	DWORD dwLeftNumber = 0;
	DWORD dwRightNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[0],
		pdwOperands[0],
		&dwLeftNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[1],
		pdwOperands[1],
		&dwRightNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwLeftNumber < dwRightNumber)
	{
		psVirtualProcessor->IP = pdwMarkerks[0];
	}
	else
	{
		psVirtualProcessor->IP = pdwMarkerks[1];
	}

	return TRUE;
}

BOOL
PasmEq4(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD pdwOperands[2];
	DWORD pdwMarkerks[2];

	memcpy(pdwOperands, pCurrentInstruction, sizeof(pdwOperands));
	memcpy(pdwMarkerks, pCurrentInstruction + sizeof(pdwOperands), sizeof(pdwMarkerks));

	EOperandTypes peTypes[2] =
	{
		RecognizeOperand(pdwOperands[0]),
		RecognizeOperand(pdwOperands[1])
	};

	DWORD dwLeftNumber = 0;
	DWORD dwRightNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[0],
		pdwOperands[0],
		&dwLeftNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[1],
		pdwOperands[1],
		&dwRightNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwLeftNumber == dwRightNumber)
	{
		psVirtualProcessor->IP = pdwMarkerks[0];
	}
	else
	{
		psVirtualProcessor->IP = pdwMarkerks[1];
	}

	return TRUE;
}

BOOL
PasmNe4(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD pdwOperands[2];
	DWORD pdwMarkerks[2];

	memcpy(pdwOperands, pCurrentInstruction, sizeof(pdwOperands));
	memcpy(pdwMarkerks, pCurrentInstruction + sizeof(pdwOperands), sizeof(pdwMarkerks));

	EOperandTypes peTypes[2] =
	{
		RecognizeOperand(pdwOperands[0]),
		RecognizeOperand(pdwOperands[1])
	};

	DWORD dwLeftNumber = 0;
	DWORD dwRightNumber = 0;

	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[0],
		pdwOperands[0],
		&dwLeftNumber);
	if (!bResult)
	{
		return FALSE;
	}

	bResult = GetNativeNumber(
		psVirtualProcessor,
		peTypes[1],
		pdwOperands[1],
		&dwRightNumber);
	if (!bResult)
	{
		return FALSE;
	}

	if (dwLeftNumber != dwRightNumber)
	{
		psVirtualProcessor->IP = pdwMarkerks[0];
	}
	else
	{
		psVirtualProcessor->IP = pdwMarkerks[1];
	}

	return TRUE;
}