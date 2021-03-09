#include "global.h"

#define OPERAND_CUT_MASK		0x7C000000
#define NUMBER_CUT_MASK			0x03FFFFFF

#define NO_SUCCESS_EXECUTION	((DWORD)-1)

static
DOUBLE
DecodeFloat(
	WORD wValue
)
{
	INT nCount = wValue >> 12;
	DOUBLE fResult = wValue & 0xFFF;
	while (nCount > 0)
	{
		fResult /= 10.0;
		nCount--;
	}

	return fResult;
}

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
	case BYTECODE_FLOAT_NUMBER_FLAG:
		return EOT_FLOAT;
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
	DWORD dwRegisterNumber = (dwOperand >> 24) & 0x3;
	DWORD dwRegisterType = dwOperand & 0xF;

	switch (dwRegisterType)
	{
	case ERT_I:
		*pbTargetMemory = &psVirtualProcessor->I[dwRegisterNumber];
		*pdwSize = sizeof(DWORD);
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
GetIntegerNumberLiteral(
	DWORD	dwOperand
)
{
	SHORT wNumber = 0;
	DWORD dwCuted = dwOperand & NUMBER_CUT_MASK;
	memcpy(&wNumber, &dwCuted, sizeof(SHORT));
	
	INT dwResult = (INT)wNumber;
	return dwResult;
}

static
INT
GetFloatNumberLiteral(
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
EOperandTypes
GetNativeNumber(
	PSVirtualProcessor	psVirtualProcessor,
	EOperandTypes		eOpType,
	DWORD				dwOperand,
	PDWORD				pdwNumber
)
{
	switch (eOpType)
	{
	case EOT_FLOAT:
	{
		DWORD dwCompressed = GetFloatNumberLiteral(dwOperand);
		FLOAT fDecoded = (FLOAT)DecodeFloat((WORD)dwCompressed);
		memcpy(pdwNumber, &fDecoded, sizeof(FLOAT));
		return EOT_FLOAT;
	}
	case EOT_NUMBER:
	{
		DWORD dwLiteral = GetIntegerNumberLiteral(dwOperand);
		memcpy(pdwNumber, &dwLiteral, sizeof(INT));
		return EOT_NUMBER;
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

		switch (eSrcRegister)
		{
		case ERT_I:
			memcpy(pdwNumber, pTargetMemory, dwSize);
			return EOT_NUMBER;
		case ERT_N:
			memcpy(pdwNumber, pTargetMemory, dwSize);
			return EOT_FLOAT;
		case ERT_P:
		{
			DWORD dwNumber = 0;
			EOperandTypes eOperandType;
			BOOL bResult = PmcGetNativeNumber(
				pTargetMemory,
				&eOperandType,
				&dwNumber);
			if (!bResult)
			{
				return FALSE;
			}

			memcpy(pdwNumber, &dwNumber, sizeof(DWORD));
			return eOperandType;
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
		case EOT_FLOAT:
		{
			if (eDestRegister == ERT_S)
			{
				return FALSE;
			}

			if (eDestRegister == ERT_P)
			{
				DWORD dwNumber = GetFloatNumberLiteral(pdwOperands[1]);
				FLOAT fDecoded = DecodeFloat((WORD)dwNumber);

				return PmcSetFloat(pTargetMemory, fDecoded);
			}
			else
			{
				DWORD dwNumber = GetFloatNumberLiteral(pdwOperands[1]);
				FLOAT fDecoded = DecodeFloat((WORD)dwNumber);
				memcpy(pTargetMemory, &fDecoded, sizeof(FLOAT));
				return TRUE;
			}
		}
		case EOT_NUMBER:
		{
			if (eDestRegister == ERT_S)
			{
				return FALSE;
			}

			if (eDestRegister == ERT_P)
			{
				DWORD dwLiteral = GetIntegerNumberLiteral(pdwOperands[1]);
				INT nNumber = 0;
				memcpy(&nNumber, &dwLiteral, sizeof(INT));

				return PmcSetInteger(pTargetMemory, nNumber);
			}
			else
			{
				DWORD dwLiteral = GetIntegerNumberLiteral(pdwOperands[1]);
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
				PCHAR pString = GetStringLitral(
					pdwOperands[1],
					psIndexTable);

				return PmcSetString(pTargetMemory, pString);
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
			DWORD dwLiteral = GetIntegerNumberLiteral(pdwOperands[1]);
			memcpy(pTargetMemory, &dwLiteral, sizeof(DWORD));
			return TRUE;
		}
		case EOT_FLOAT:
		{
			DWORD dwNumber = GetFloatNumberLiteral(pdwOperands[1]);
			FLOAT fDecoded = DecodeFloat((WORD)dwNumber);
			memcpy(pTargetMemory, &fDecoded, sizeof(FLOAT));
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
	{
		INT nNumber = *(INT*)pTargetMemory;
		nNumber++;

		memcpy(pTargetMemory, &nNumber, sizeof(INT));
		return TRUE;
	}
	case ERT_N:
	{
		FLOAT nNumber = *(FLOAT*)pTargetMemory;
		nNumber++;

		memcpy(pTargetMemory, &nNumber, sizeof(FLOAT));
		return TRUE;
	}
	case ERT_P:
		return PmcIncrement(pTargetMemory);
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
	{
		INT nNumber = *(INT*)pTargetMemory;
		nNumber--;

		memcpy(pTargetMemory, &nNumber, sizeof(INT));
		return TRUE;
	}
	case ERT_N:
	{
		FLOAT nNumber = *(FLOAT*)pTargetMemory;
		nNumber--;

		memcpy(pTargetMemory, &nNumber, sizeof(FLOAT));
		return TRUE;
	}
	case ERT_P:
		return PmcDecrement(pTargetMemory);
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

	EOperandTypes eLeft = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[0],
		dwOperand[1],
		&dwFirstNumber);
	if (!eLeft)
	{
		return FALSE;
	}
	
	EOperandTypes eRight = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[1],
		dwOperand[2],
		&dwSecondNumber);
	if (!eRight)
	{
		return FALSE;
	}

	DWORD dwResultNumber = 0;
	EOperandTypes eTargetOpType = 0;
	switch (eLeft)
	{
	case EOT_FLOAT:
	{
		switch (eRight)
		{
		case EOT_FLOAT:
		{
			FLOAT fLeft;
			FLOAT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(FLOAT));
			memcpy(&fRight, &dwSecondNumber, sizeof(FLOAT));

			fLeft += fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		case EOT_NUMBER:
		{
			FLOAT fLeft;
			INT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(FLOAT));
			memcpy(&fRight, &dwSecondNumber, sizeof(DWORD));

			fLeft += fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		default:
			return FALSE;
		}
		break;
	}
	case EOT_NUMBER:
	{
		switch (eRight)
		{
		case EOT_FLOAT:
		{
			INT fLeft;
			FLOAT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(DWORD));
			memcpy(&fRight, &dwSecondNumber, sizeof(FLOAT));

			fRight += fLeft;
			memcpy(&dwResultNumber, &fRight, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		case EOT_NUMBER:
		{
			INT fLeft;
			INT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(DWORD));
			memcpy(&fRight, &dwSecondNumber, sizeof(DWORD));

			fLeft += fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_NUMBER;

			break;
		}
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
	{
		switch (eTargetOpType)
		{
		case EOT_FLOAT:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&nResult, &dwResultNumber, sizeof(FLOAT));

			dwSrc = (INT)nResult;
			memcpy(pTargetMemory, &dwSrc, sizeof(INT));

			return TRUE;
		}
		case EOT_NUMBER:
		{
			memcpy(pTargetMemory, &dwResultNumber, sizeof(INT));

			return TRUE;
		}
		default:
			return FALSE;
		}
	}
	case ERT_N:
		switch (eTargetOpType)
		{
		case EOT_NUMBER:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&dwSrc, &dwResultNumber, sizeof(INT));

			nResult = (FLOAT)dwSrc;
			memcpy(pTargetMemory, &nResult, sizeof(FLOAT));

			return TRUE;
		}
		case EOT_FLOAT:
		{
			memcpy(pTargetMemory, &dwResultNumber, sizeof(FLOAT));

			return TRUE;
		}
		default:
			return FALSE;
		}
	case ERT_P:
		switch (eTargetOpType)
		{
		case EOT_NUMBER:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&dwSrc, &dwResultNumber, sizeof(INT));

			nResult = (FLOAT)dwSrc;

			return PmcSetInteger(pTargetMemory, nResult);
		}
		case EOT_FLOAT:
		{
			FLOAT nResult = 0;
			memcpy(&nResult, &dwResultNumber, sizeof(FLOAT));

			return PmcSetFloat(pTargetMemory, nResult);;
		}
		default:
			return FALSE;
		}
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

	EOperandTypes eLeft = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[0],
		dwOperand[1],
		&dwFirstNumber);
	if (!eLeft)
	{
		return FALSE;
	}

	EOperandTypes eRight = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[1],
		dwOperand[2],
		&dwSecondNumber);
	if (!eRight)
	{
		return FALSE;
	}

	DWORD dwResultNumber = 0;
	EOperandTypes eTargetOpType = 0;
	switch (eLeft)
	{
	case EOT_FLOAT:
	{
		switch (eRight)
		{
		case EOT_FLOAT:
		{
			FLOAT fLeft;
			FLOAT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(FLOAT));
			memcpy(&fRight, &dwSecondNumber, sizeof(FLOAT));

			fLeft -= fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		case EOT_NUMBER:
		{
			FLOAT fLeft;
			INT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(FLOAT));
			memcpy(&fRight, &dwSecondNumber, sizeof(DWORD));

			fLeft -= fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		default:
			return FALSE;
		}
		break;
	}
	case EOT_NUMBER:
	{
		switch (eRight)
		{
		case EOT_FLOAT:
		{
			INT fLeft;
			FLOAT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(DWORD));
			memcpy(&fRight, &dwSecondNumber, sizeof(FLOAT));

			fRight -= fLeft;
			memcpy(&dwResultNumber, &fRight, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		case EOT_NUMBER:
		{
			INT fLeft;
			INT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(DWORD));
			memcpy(&fRight, &dwSecondNumber, sizeof(DWORD));

			fLeft -= fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_NUMBER;

			break;
		}
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
	{
		switch (eTargetOpType)
		{
		case EOT_FLOAT:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&nResult, &dwResultNumber, sizeof(FLOAT));

			dwSrc = (INT)nResult;
			memcpy(pTargetMemory, &dwSrc, sizeof(INT));

			return TRUE;
		}
		case EOT_NUMBER:
		{
			memcpy(pTargetMemory, &dwResultNumber, sizeof(INT));

			return TRUE;
		}
		default:
			return FALSE;
		}
		break;
	}
	case ERT_N:
		switch (eTargetOpType)
		{
		case EOT_NUMBER:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&dwSrc, &dwResultNumber, sizeof(INT));

			nResult = (FLOAT)dwSrc;
			memcpy(pTargetMemory, &nResult, sizeof(FLOAT));

			return TRUE;
		}
		case EOT_FLOAT:
		{
			memcpy(pTargetMemory, &dwResultNumber, sizeof(FLOAT));

			return TRUE;
		}
		default:
			return FALSE;
		}
	case ERT_P:
		switch (eTargetOpType)
		{
		case EOT_NUMBER:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&dwSrc, &dwResultNumber, sizeof(INT));

			nResult = (FLOAT)dwSrc;

			return PmcSetInteger(pTargetMemory, nResult);
		}
		case EOT_FLOAT:
		{
			FLOAT nResult = 0;
			memcpy(&nResult, &dwResultNumber, sizeof(FLOAT));

			return PmcSetFloat(pTargetMemory, nResult);;
		}
		default:
			return FALSE;
		}
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

	EOperandTypes eLeft = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[0],
		dwOperand[1],
		&dwFirstNumber);
	if (!eLeft)
	{
		return FALSE;
	}

	EOperandTypes eRight = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[1],
		dwOperand[2],
		&dwSecondNumber);
	if (!eRight)
	{
		return FALSE;
	}

	DWORD dwResultNumber = 0;
	EOperandTypes eTargetOpType = 0;
	switch (eLeft)
	{
	case EOT_FLOAT:
	{
		switch (eRight)
		{
		case EOT_FLOAT:
		{
			FLOAT fLeft;
			FLOAT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(FLOAT));
			memcpy(&fRight, &dwSecondNumber, sizeof(FLOAT));

			fLeft *= fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		case EOT_NUMBER:
		{
			FLOAT fLeft;
			INT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(FLOAT));
			memcpy(&fRight, &dwSecondNumber, sizeof(DWORD));

			fLeft *= fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		default:
			return FALSE;
		}
		break;
	}
	case EOT_NUMBER:
	{
		switch (eRight)
		{
		case EOT_FLOAT:
		{
			INT fLeft;
			FLOAT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(DWORD));
			memcpy(&fRight, &dwSecondNumber, sizeof(FLOAT));

			fRight *= fLeft;
			memcpy(&dwResultNumber, &fRight, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		case EOT_NUMBER:
		{
			INT fLeft;
			INT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(DWORD));
			memcpy(&fRight, &dwSecondNumber, sizeof(DWORD));

			fLeft *= fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_NUMBER;

			break;
		}
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
	{
		switch (eTargetOpType)
		{
		case EOT_FLOAT:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&nResult, &dwResultNumber, sizeof(FLOAT));

			dwSrc = (INT)nResult;
			memcpy(pTargetMemory, &dwSrc, sizeof(INT));

			return TRUE;
		}
		case EOT_NUMBER:
		{
			memcpy(pTargetMemory, &dwResultNumber, sizeof(INT));

			return TRUE;
		}
		default:
			return FALSE;
		}
		break;
	}
	case ERT_N:
		switch (eTargetOpType)
		{
		case EOT_NUMBER:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&dwSrc, &dwResultNumber, sizeof(INT));

			nResult = (FLOAT)dwSrc;
			memcpy(pTargetMemory, &nResult, sizeof(FLOAT));

			return TRUE;
		}
		case EOT_FLOAT:
		{
			memcpy(pTargetMemory, &dwResultNumber, sizeof(FLOAT));

			return TRUE;
		}
		default:
			return FALSE;
		}
	case ERT_P:
		switch (eTargetOpType)
		{
		case EOT_NUMBER:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&dwSrc, &dwResultNumber, sizeof(INT));

			nResult = (FLOAT)dwSrc;

			return PmcSetInteger(pTargetMemory, nResult);
		}
		case EOT_FLOAT:
		{
			FLOAT nResult = 0;
			memcpy(&nResult, &dwResultNumber, sizeof(FLOAT));

			return PmcSetFloat(pTargetMemory, nResult);;
		}
		default:
			return FALSE;
		}
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

	EOperandTypes eLeft = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[0],
		dwOperand[1],
		&dwFirstNumber);
	if (!eLeft)
	{
		return FALSE;
	}

	EOperandTypes eRight = GetNativeNumber(
		psVirtualProcessor,
		eOperandType[1],
		dwOperand[2],
		&dwSecondNumber);
	if (!eRight)
	{
		return FALSE;
	}

	DWORD dwResultNumber = 0;
	EOperandTypes eTargetOpType = 0;
	switch (eLeft)
	{
	case EOT_FLOAT:
	{
		switch (eRight)
		{
		case EOT_FLOAT:
		{
			FLOAT fLeft;
			FLOAT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(FLOAT));
			memcpy(&fRight, &dwSecondNumber, sizeof(FLOAT));

			fLeft /= fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		case EOT_NUMBER:
		{
			FLOAT fLeft;
			INT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(FLOAT));
			memcpy(&fRight, &dwSecondNumber, sizeof(DWORD));

			fLeft /= fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		default:
			return FALSE;
		}
		break;
	}
	case EOT_NUMBER:
	{
		switch (eRight)
		{
		case EOT_FLOAT:
		{
			INT fLeft;
			FLOAT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(DWORD));
			memcpy(&fRight, &dwSecondNumber, sizeof(FLOAT));

			fRight /= fLeft;
			memcpy(&dwResultNumber, &fRight, sizeof(DWORD));
			eTargetOpType = EOT_FLOAT;

			break;
		}
		case EOT_NUMBER:
		{
			INT fLeft;
			INT fRight;

			memcpy(&fLeft, &dwFirstNumber, sizeof(DWORD));
			memcpy(&fRight, &dwSecondNumber, sizeof(DWORD));

			fLeft /= fRight;
			memcpy(&dwResultNumber, &fLeft, sizeof(DWORD));
			eTargetOpType = EOT_NUMBER;

			break;
		}
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
	{
		switch (eTargetOpType)
		{
		case EOT_FLOAT:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&nResult, &dwResultNumber, sizeof(FLOAT));

			dwSrc = (INT)nResult;
			memcpy(pTargetMemory, &dwSrc, sizeof(INT));

			return TRUE;
		}
		case EOT_NUMBER:
		{
			memcpy(pTargetMemory, &dwResultNumber, sizeof(INT));

			return TRUE;
		}
		default:
			return FALSE;
		}
		break;
	}
	case ERT_N:
		switch (eTargetOpType)
		{
		case EOT_NUMBER:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&dwSrc, &dwResultNumber, sizeof(INT));

			nResult = (FLOAT)dwSrc;
			memcpy(pTargetMemory, &nResult, sizeof(FLOAT));

			return TRUE;
		}
		case EOT_FLOAT:
		{
			memcpy(pTargetMemory, &dwResultNumber, sizeof(FLOAT));

			return TRUE;
		}
		default:
			return FALSE;
		}
	case ERT_P:
		switch (eTargetOpType)
		{
		case EOT_NUMBER:
		{
			INT dwSrc = 0;
			FLOAT nResult = 0;
			memcpy(&dwSrc, &dwResultNumber, sizeof(INT));

			nResult = (FLOAT)dwSrc;

			return PmcSetInteger(pTargetMemory, nResult);
		}
		case EOT_FLOAT:
		{
			FLOAT nResult = 0;
			memcpy(&nResult, &dwResultNumber, sizeof(FLOAT));

			return PmcSetFloat(pTargetMemory, nResult);;
		}
		default:
			return FALSE;
		}
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

	DWORD dwNumber = 0;
	EOperandTypes opType = RecognizeOperand(dwOperand[1]);
	EOperandTypes opResult = GetNativeNumber(
		psVirtualProcessor,
		opType, dwOperand[1],
		&dwNumber);
	if (!opResult)
	{
		return FALSE;
	}

	switch (eRegisterType)
	{
	case ERT_I:
	{
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			INT nDest = *(INT*)pTargetMemory;

			FLOAT fNumber = 0;
			memcpy(&fNumber, &dwNumber, sizeof(FLOAT));

			nDest += fNumber;
			memcpy(pTargetMemory, &nDest, sizeof(INT));
			return TRUE;
		}
		case EOT_NUMBER:
		{
			INT nDest = *(INT*)pTargetMemory;

			INT nNumber = 0;
			memcpy(&nNumber, &dwNumber, sizeof(INT));

			nDest += nNumber;
			memcpy(pTargetMemory, &nDest, sizeof(INT));
			return TRUE;
		}
		default:
			return FALSE;
		}
	}
	case ERT_N:
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			FLOAT fNumber = *(FLOAT*)pTargetMemory;

			FLOAT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(FLOAT));

			fNumber += nSrc;
			memcpy(pTargetMemory, &fNumber, sizeof(FLOAT));
			return TRUE;
		}
		case EOT_NUMBER:
		{
			FLOAT fNumber = *(FLOAT*)pTargetMemory;
			
			INT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(INT));

			fNumber += nSrc;
			memcpy(pTargetMemory, &fNumber, sizeof(FLOAT));
			return TRUE;
		}
		default:
			return FALSE;
		}
	case ERT_P:
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			FLOAT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(FLOAT));

			return PmcAddFloat(pTargetMemory, nSrc);
		}
		case EOT_NUMBER:
		{
			INT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(INT));

			return PmcAddInteger(pTargetMemory, nSrc);
		}
		default:
			return FALSE;
		}
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

	DWORD dwNumber = 0;
	EOperandTypes opType = RecognizeOperand(dwOperand[1]);
	EOperandTypes opResult = GetNativeNumber(
		psVirtualProcessor,
		opType, dwOperand[1],
		&dwNumber);
	if (!opResult)
	{
		return FALSE;
	}

	switch (eRegisterType)
	{
	case ERT_I:
	{
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			INT nDest = *(INT*)pTargetMemory;

			FLOAT fNumber = 0;
			memcpy(&fNumber, &dwNumber, sizeof(FLOAT));

			nDest -= fNumber;
			memcpy(pTargetMemory, &nDest, sizeof(INT));
			return TRUE;
		}
		case EOT_NUMBER:
		{
			INT nDest = *(INT*)pTargetMemory;

			INT nNumber = 0;
			memcpy(&nNumber, &dwNumber, sizeof(INT));

			nDest -= nNumber;
			memcpy(pTargetMemory, &nDest, sizeof(INT));
			return TRUE;
		}
		default:
			return FALSE;
		}
	}
	case ERT_N:
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			FLOAT fNumber = *(FLOAT*)pTargetMemory;

			FLOAT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(FLOAT));

			fNumber -= nSrc;
			memcpy(pTargetMemory, &fNumber, sizeof(FLOAT));
			return TRUE;
		}
		case EOT_NUMBER:
		{
			FLOAT fNumber = *(FLOAT*)pTargetMemory;

			INT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(INT));

			fNumber -= nSrc;
			memcpy(pTargetMemory, &fNumber, sizeof(FLOAT));
			return TRUE;
		}
		default:
			return FALSE;
		}
	case ERT_P:
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			FLOAT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(FLOAT));

			return PmcSubFloat(pTargetMemory, nSrc);
		}
		case EOT_NUMBER:
		{
			INT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(INT));

			return PmcSubInteger(pTargetMemory, nSrc);
		}
		default:
			return FALSE;
		}
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

	DWORD dwNumber = 0;
	EOperandTypes opType = RecognizeOperand(dwOperand[1]);
	EOperandTypes opResult = GetNativeNumber(
		psVirtualProcessor,
		opType, dwOperand[1],
		&dwNumber);
	if (!opResult)
	{
		return FALSE;
	}

	switch (eRegisterType)
	{
	case ERT_I:
	{
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			INT nDest = *(INT*)pTargetMemory;

			FLOAT fNumber = 0;
			memcpy(&fNumber, &dwNumber, sizeof(FLOAT));

			nDest *= fNumber;
			memcpy(pTargetMemory, &nDest, sizeof(INT));
			return TRUE;
		}
		case EOT_NUMBER:
		{
			INT nDest = *(INT*)pTargetMemory;

			INT nNumber = 0;
			memcpy(&nNumber, &dwNumber, sizeof(INT));

			nDest *= nNumber;
			memcpy(pTargetMemory, &nDest, sizeof(INT));
			return TRUE;
		}
		default:
			return FALSE;
		}
	}
	case ERT_N:
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			FLOAT fNumber = *(FLOAT*)pTargetMemory;

			FLOAT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(FLOAT));

			fNumber *= nSrc;
			memcpy(pTargetMemory, &fNumber, sizeof(FLOAT));
			return TRUE;
		}
		case EOT_NUMBER:
		{
			FLOAT fNumber = *(FLOAT*)pTargetMemory;

			INT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(INT));

			fNumber *= nSrc;
			memcpy(pTargetMemory, &fNumber, sizeof(FLOAT));
			return TRUE;
		}
		default:
			return FALSE;
		}
	case ERT_P:
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			FLOAT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(FLOAT));

			return PmcMulFloat(pTargetMemory, nSrc);
		}
		case EOT_NUMBER:
		{
			INT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(INT));

			return PmcMulInteger(pTargetMemory, nSrc);
		}
		default:
			return FALSE;
		}
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

	DWORD dwNumber = 0;
	EOperandTypes opType = RecognizeOperand(dwOperand[1]);
	EOperandTypes opResult = GetNativeNumber(
		psVirtualProcessor,
		opType, dwOperand[1],
		&dwNumber);
	if (!opResult)
	{
		return FALSE;
	}

	switch (eRegisterType)
	{
	case ERT_I:
	{
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			INT nDest = *(INT*)pTargetMemory;

			FLOAT fNumber = 0;
			memcpy(&fNumber, &dwNumber, sizeof(FLOAT));

			nDest /= fNumber;
			memcpy(pTargetMemory, &nDest, sizeof(INT));
			return TRUE;
		}
		case EOT_NUMBER:
		{
			INT nDest = *(INT*)pTargetMemory;

			INT nNumber = 0;
			memcpy(&nNumber, &dwNumber, sizeof(INT));

			nDest /= nNumber;
			memcpy(pTargetMemory, &nDest, sizeof(INT));
			return TRUE;
		}
		default:
			return FALSE;
		}
	}
	case ERT_N:
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			FLOAT fNumber = *(FLOAT*)pTargetMemory;

			FLOAT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(FLOAT));

			fNumber /= nSrc;
			memcpy(pTargetMemory, &fNumber, sizeof(FLOAT));
			return TRUE;
		}
		case EOT_NUMBER:
		{
			FLOAT fNumber = *(FLOAT*)pTargetMemory;

			INT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(INT));

			fNumber /= nSrc;
			memcpy(pTargetMemory, &fNumber, sizeof(FLOAT));
			return TRUE;
		}
		default:
			return FALSE;
		}
	case ERT_P:
		switch (opResult)
		{
		case EOT_FLOAT:
		{
			FLOAT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(FLOAT));

			return PmcDivFloat(pTargetMemory, nSrc);
		}
		case EOT_NUMBER:
		{
			INT nSrc = 0;
			memcpy(&nSrc, &dwNumber, sizeof(INT));

			return PmcDivInteger(pTargetMemory, nSrc);
		}
		default:
			return FALSE;
		}
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
	case EOT_FLOAT:
	{
		DWORD dwNumber = GetFloatNumberLiteral(dwOperand);
		FLOAT fDecoded = DecodeFloat((WORD)dwNumber);
		printf("%f", fDecoded);

		return TRUE;
	}
	case EOT_NUMBER:
	{
		DWORD dwNumber = GetIntegerNumberLiteral(dwOperand);
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
		{
			DWORD dwNumber = 0;
			memcpy(&dwNumber, pSrcMemory, dwSrcSize);
			printf("%d", dwNumber);

			return TRUE;
		}
		case ERT_N:
		{
			FLOAT fNumber = 0;
			memcpy(&fNumber, pSrcMemory, dwSrcSize);
			printf("%f", fNumber);

			return TRUE;
		}
		case ERT_S:
		{
			printf("%s", (PCHAR)pSrcMemory);
			return TRUE;
		}
		case ERT_P:
		{
			return PmcPrint(pSrcMemory);
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

	DWORD dwDestSize = 0;
	PCHAR pString = NULL;
	EOperandTypes eOperandType = RecognizeOperand(dwOperand[1]);
	switch (eOperandType)
	{
	case EOT_REGISTER:
		bResult = RecognizeRegister(
			dwOperand[1],
			psVirtualProcessor,
			&pString,
			&dwDestSize);
		if (!bResult)
		{
			return FALSE;
		}
		
		if (bResult == ERT_P)
		{
			pString = PmcGetString(pString);
			if (!pString)
			{
				return FALSE;
			}
		}
		break;
	case EOT_STRING:
		pString = GetStringLitral(
			dwOperand[1],
			psIndexTable);
		break;
	default:
		return FALSE;
	}

	SIZE_T nLength = strlen(pString);
	memcpy(pTargetMemory, &nLength, dwSize);

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

BOOL
PasmBsr(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pStack
)
{
	DWORD dwMarker;
	memcpy(&dwMarker, pCurrentInstruction, sizeof(DWORD));

	DWORD dwNextInsruction = psVirtualProcessor->IP + sizeof(DWORD) + sizeof(BYTE);
	memcpy(pStack + psVirtualProcessor->SP, &dwNextInsruction, sizeof(DWORD));

	psVirtualProcessor->IP = dwMarker;
	psVirtualProcessor->SP += sizeof(dwMarker);

	return TRUE;
}

BOOL
PasmRet(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pStack
)
{
	DWORD dwMarker;
	psVirtualProcessor->SP -= sizeof(DWORD);
	memcpy(&dwMarker, pStack + psVirtualProcessor->SP, sizeof(DWORD));

	psVirtualProcessor->IP = dwMarker;
	return TRUE;
}

BOOL
PasmPushStack(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pStack
)
{
	DWORD dwOperand;
	memcpy(&dwOperand, pCurrentInstruction, sizeof(DWORD));

	EOperandTypes eOperandType = RecognizeOperand(dwOperand);

	DWORD dwNumber = 0;
	BOOL bResult = GetNativeNumber(
		psVirtualProcessor,
		eOperandType,
		dwOperand,
		&dwNumber);
	if (!bResult)
	{
		return FALSE;
	}

	memcpy(pStack + psVirtualProcessor->SP, &dwNumber, sizeof(DWORD));
	psVirtualProcessor->SP += sizeof(DWORD);

	return TRUE;
}

BOOL
PasmPopStack(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pStack
)
{
	DWORD dwOperand;
	memcpy(&dwOperand, pCurrentInstruction, sizeof(DWORD));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	EOperandTypes eOperandType = RecognizeOperand(dwOperand);
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand,
		psVirtualProcessor,
		&pTargetMemory,
		&dwSize);
	if (!eRegisterType)
	{
		return FALSE;
	}

	psVirtualProcessor->SP -= dwSize;
	memcpy(pTargetMemory, pStack + psVirtualProcessor->SP, dwSize);
	
	return TRUE;
}

BOOL
PasmNew(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[2];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegisterType = RecognizeRegister(
		dwOperand[0],
		psVirtualProcessor,
		&pTargetMemory,
		&dwSize);
	if (eRegisterType != ERT_P)
	{
		return FALSE;
	}

	return PmcNew(pTargetMemory, dwOperand[1]);
}

BOOL
PasmPushHashTable(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pIndexTable,
	PBYTE				pVirtualMemory
)
{
	DWORD dwOperand[3];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eRegType = RecognizeRegister(
		dwOperand[0],
		psVirtualProcessor,
		&pTargetMemory,
		&dwSize);
	if (eRegType != ERT_P)
	{
		return FALSE;
	}

	EPmcType ePmcType = PmcGetType(pTargetMemory);
	if (ePmcType != EPMCT_HASHTABLE)
	{
		return FALSE;
	}

	PCHAR pszKey = ExtractString(
		dwOperand[1],
		psVirtualProcessor,
		pIndexTable);
	if (!pszKey)
	{
		return FALSE;
	}

	UPmcData pInsertMemory;
	EOperandTypes eInsertType = RecognizeOperand(dwOperand[2]);
	EOperandTypes eTargetType = eInsertType;
	switch (eInsertType)
	{
	case EOT_SPECIAL:
	case EOT_VIRTUAL_MEMORY:
		return FALSE;
	case EOT_FLOAT:
	{
		DWORD dwCompressed = GetFloatNumberLiteral(dwOperand[2]);
		FLOAT fDecoded = (FLOAT)DecodeFloat((WORD)dwCompressed);
		memcpy(&pInsertMemory.Float, &fDecoded, sizeof(FLOAT));
		break;
	}
	case EOT_NUMBER:
	{
		INT nNumber = GetIntegerNumberLiteral(dwOperand[2]);
		memcpy(&pInsertMemory.Integer, &nNumber, sizeof(INT));
		break;
	}
	case EOT_STRING:
	{
		PCHAR pString = GetStringLitral(
			dwOperand[2],
			pIndexTable);
		size_t nLength = strlen(pString);
		memcpy(&pInsertMemory.String, pString, nLength + 1);
		break;
	}
	case EOT_REGISTER:
	{
		DWORD dwInsertSize = 0;
		PBYTE pInsertRegMemory = NULL;
		ERegisterTypes eRegType = RecognizeRegister(
			dwOperand[2],
			psVirtualProcessor,
			&pInsertRegMemory,
			&dwInsertSize);
		if (!eRegType)
		{
			return FALSE;
		}


		switch (eRegType)
		{
		case ERT_I:
			eTargetType = EOT_NUMBER;
			memcpy(&pInsertMemory.Integer, pInsertRegMemory, dwInsertSize);
			break;
		case ERT_N:
			eTargetType = EOT_FLOAT;
			memcpy(&pInsertMemory.Float, pInsertRegMemory, dwInsertSize);
			break;
		case ERT_S:
			eTargetType = EOT_STRING;
			memcpy(&pInsertMemory.String, pInsertRegMemory, dwInsertSize);
			break;
		case ERT_P:
		{
			eTargetType = PmcRecognizeOperand(
				pInsertRegMemory,
				&pInsertMemory);
			if (!eTargetType)
			{
				return FALSE;
			}
			break;
		}
		default:
			return FALSE;
		}
		break;
	}
	default:
		return FALSE;
	}

	return PmcHashTableInsert(
		pTargetMemory,
		pszKey,
		eTargetType,
		&pInsertMemory);
}

BOOL
PasmPopHashTable(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pIndexTable,
	PBYTE				pVirtualMemory
)
{
	DWORD dwOperand[3];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	psVirtualProcessor->FLAG = TRUE;

	DWORD dwSize = 0;
	PBYTE pTargetMemory = NULL;
	ERegisterTypes eType = RecognizeRegister(
		dwOperand[0],
		psVirtualProcessor,
		&pTargetMemory,
		&dwSize);
	if (!eType)
	{
		return FALSE;
	}

	DWORD dwPmcSize = 0;
	PBYTE pPmcRegister = NULL;
	ERegisterTypes eRegType = RecognizeRegister(
		dwOperand[1],
		psVirtualProcessor,
		&pPmcRegister,
		&dwPmcSize);
	if (!eRegType || eRegType != ERT_P)
	{
		return FALSE;
	}

	PCHAR pszKey = ExtractString(
		dwOperand[2],
		psVirtualProcessor,
		pIndexTable);
	if (!pszKey)
	{
		return FALSE;
	}

	BOOL bFound = TRUE;
	BOOL bResult = PmcHashTableFind(
		pPmcRegister,
		pszKey,
		eType,
		pTargetMemory,
		&bFound);
	if (!bFound)
	{
		psVirtualProcessor->FLAG = FALSE;
	}

	return bResult;
}

BOOL
PasmEraseHashTable(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pIndexTable
)
{
	DWORD dwOperand[2];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	psVirtualProcessor->FLAG = TRUE;

	DWORD dwPmcSize = 0;
	PBYTE pPmcRegister = NULL;
	ERegisterTypes eRegType = RecognizeRegister(
		dwOperand[0],
		psVirtualProcessor,
		&pPmcRegister,
		&dwPmcSize);
	if (!eRegType || eRegType != ERT_P)
	{
		return FALSE;
	}

	PCHAR pszKey = ExtractString(
		dwOperand[1],
		psVirtualProcessor,
		pIndexTable);
	if (!pszKey)
	{
		return FALSE;
	}

	BOOL bRemoved = TRUE;
	PmcHashTableRemove(
		pPmcRegister,
		pszKey,
		&bRemoved);

	if (!bRemoved)
	{
		psVirtualProcessor->FLAG = FALSE;
	}

	return TRUE;
}

BOOL
PasmChk(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	if (!psVirtualProcessor->FLAG)
	{
		DWORD dwMarker;
		memcpy(&dwMarker, pCurrentInstruction, sizeof(DWORD));

		psVirtualProcessor->IP = dwMarker;

		psVirtualProcessor->FLAG = TRUE;
	}
	else
	{
		psVirtualProcessor->IP += 5;
	}

	return TRUE;
}

BOOL
PasmTypeof(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
)
{
	DWORD dwOperand[2];
	memcpy(dwOperand, pCurrentInstruction, sizeof(dwOperand));

	DWORD dwSize = 0;
	PBYTE pRegister = NULL;
	ERegisterTypes eRegType = RecognizeRegister(
		dwOperand[1],
		psVirtualProcessor,
		&pRegister,
		&dwSize);
	if (!eRegType || eRegType != ERT_S)
	{
		return FALSE;
	}

	DWORD dwPmcSize = 0;
	PBYTE pPmcRegister = NULL;
	eRegType = RecognizeRegister(
		dwOperand[1],
		psVirtualProcessor,
		&pPmcRegister,
		&dwPmcSize);
	if (!eRegType || eRegType != ERT_P)
	{
		return FALSE;
	}

	EPmcType ePmcType = PmcGetType(pPmcRegister);
	switch (ePmcType)
	{
	case EPMCT_FLOAT:
		strcpy(pRegister, "Float");
		break;
	case EPMCT_INTEGER:
		strcpy(pRegister, "Integer");
		break;
	case EPMCT_STRING:
		strcpy(pRegister, "String");
		break;
	case EPMCT_HASHTABLE:
		strcpy(pRegister, "Hashtable");
		break;
	case EPMCT_UNINITIALIZED:
		strcpy(pRegister, "Uninitialized");
		break;
	default:
		return FALSE;
	}

	return TRUE;
}