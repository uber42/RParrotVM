#include "global.h"


static const DWORD dwOperationStepMap[] =
{
	-1, 9, 9, 5, 5, 9, 13, 9,
	13, 9, 13, 9, 13, 9, 9,
	17, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0,
	5, 5, 5, 13, 13, 9, 1, 9, 0
};

/**
 * Виртуальная оперативная память
 */
static BYTE*				g_bVirtualMemory = NULL;

/**
 * Виртуальный процессор
 */
static SVirtualProcessor	g_sVirtualProcessor = { 0 };

/**
 * Контекст времени исполнения 
 */
static SRuntimeContext		g_sRuntimeContext = { 0 };

BOOL
InititalizeRuntime(
	DWORD	dwMemorySize,
	DWORD	dwStackSize,
	DWORD	dwHeapSize
)
{
	assert(dwMemorySize > dwStackSize);

	g_bVirtualMemory = malloc(dwMemorySize * sizeof(BYTE));
	if (!g_bVirtualMemory)
	{
		return FALSE;
	}

	g_sRuntimeContext.dwMemorySize = dwMemorySize;
	g_sRuntimeContext.dwStackSize = dwStackSize;
	g_sRuntimeContext.dwHeapSize = dwHeapSize;

	g_sRuntimeContext.pStack = g_bVirtualMemory;
	g_sRuntimeContext.pCodeMap = g_bVirtualMemory + dwStackSize;

	for (DWORD i = 0; i < sizeof(g_sVirtualProcessor.P) / sizeof(SParrotMagicCookie); i++)
	{
		PmcInitialize(&g_sVirtualProcessor.P[i]);
	}

	g_sVirtualProcessor.FLAG = TRUE;
		
	return TRUE;
}

BOOL
LoadProgram(
	CHAR	szFileName[MAX_PATH]
)
{
	FILE* pFile = fopen(szFileName, "rb");
	if (!pFile)
	{
		return FALSE;
	}

	fseek(pFile, 0, SEEK_END);
	SIZE_T nFileSize = ftell(pFile);

	/** Достаточно ли оперативной памяти в виртуальной машине */
	if (nFileSize + g_sRuntimeContext.dwStackSize + g_sRuntimeContext.dwHeapSize > g_sRuntimeContext.dwMemorySize)
	{
		fclose(pFile);
		return FALSE;
	}

	fseek(pFile, 0, SEEK_SET);
	SIZE_T nCountRead = fread(g_bVirtualMemory + g_sRuntimeContext.dwStackSize, sizeof(BYTE), nFileSize, pFile);
	if (nCountRead != nFileSize)
	{
		fclose(pFile);
		return FALSE;
	}

	g_sRuntimeContext.pHeap = g_sRuntimeContext.pCodeMap + nFileSize;
	g_sRuntimeContext.pIndexTable = g_sRuntimeContext.pCodeMap + sizeof(SFrozenFileHeader);

	SFrozenFileHeader sHeader = { 0 };
	memcpy(&sHeader, g_sRuntimeContext.pCodeMap, sizeof(SFrozenFileHeader));

	g_sRuntimeContext.pInstruction = g_sRuntimeContext.pCodeMap + sHeader.dwCodeMapOffset;

	return TRUE;
}

BOOL
StartProgram()
{
	DWORD dwInstNumber = 1;
	for (; g_sRuntimeContext.pInstruction + g_sVirtualProcessor.IP < g_sRuntimeContext.pHeap;)
	{
		PBYTE pCurrentInstruction = g_sRuntimeContext.pInstruction + g_sVirtualProcessor.IP;
		EProcessorOperation eOperation = 0;
		memcpy(&eOperation, pCurrentInstruction, sizeof(BYTE));

		BOOL  bResult = FALSE;
		switch (eOperation)
		{
		case EPO_NEW:
			bResult = PasmNew(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_SET:
			bResult = PasmSet(
				&g_sVirtualProcessor, g_sRuntimeContext.pHeap,
				g_sRuntimeContext.pIndexTable,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_INC:
			bResult = PasmInc(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_DEC:
			bResult = PasmDec(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_ADD_2:
			bResult = PasmAdd2(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_ADD_3:
			bResult = PasmAdd3(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_SUB_2:
			bResult = PasmSub2(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_SUB_3:
			bResult = PasmSub3(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_MUL_2:
			bResult = PasmMul2(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_MUL_3:
			bResult = PasmMul3(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_DIV_2:
			bResult = PasmDiv2(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_DIV_3:
			bResult = PasmDiv3(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_LENGTH:
			bResult = PasmLength(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pIndexTable);
			break;
		case EPO_CONCAT:
			bResult = PasmConcat(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pIndexTable);
			break;
		case EPO_SUBSTR:
			bResult = PasmSubstr(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pIndexTable);
			break;
		case EPO_BRANCH:
			bResult = PasmBranch(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_IF_2:
			bResult = PasmIf2(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_IF_3:
			bResult = PasmIf3(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_NE_3:
			bResult = PasmNe3(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_NE_4:
			bResult = PasmNe4(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_EQ_3:
			bResult = PasmEq3(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_EQ_4:
			bResult = PasmEq4(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_GT_3:
			bResult = PasmGt3(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_GT_4:
			bResult = PasmGt4(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_LT_3:
			bResult = PasmLt3(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_LT_4:
			bResult = PasmLt4(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_BSR:
			bResult = PasmBsr(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pStack);
			break;
		case EPO_RET:
			bResult = PasmRet(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pStack);
			break;
		case EPO_PRINT:
			bResult = PasmPrint(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pIndexTable);
			break;
		case EPO_PUSH_STACK:
			bResult = PasmPushStack(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pStack);
			break;
		case EPO_POP_STACK:
			bResult = PasmPopStack(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pStack);
			break;
		case EPO_PMC_PUSH:
			bResult = PasmPushHashTable(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pIndexTable,
				g_sRuntimeContext.pHeap);
			break;
		case EPO_PMC_POP:
			bResult = PasmPopHashTable(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pIndexTable,
				g_sRuntimeContext.pHeap);
			break;
		case EPO_PMC_ERASE:
			bResult = PasmEraseHashTable(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE),
				g_sRuntimeContext.pIndexTable);
			break;
		case EPO_END:
			return TRUE;
		case EPO_TYPEOF:
			bResult = PasmTypeof(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		case EPO_CHK:
			bResult = PasmChk(
				&g_sVirtualProcessor,
				pCurrentInstruction + sizeof(BYTE));
			break;
		}

		if (!bResult)
		{
			CHAR szMessage[STRING_MAX_LENGTH];
			sprintf(szMessage, "Строка №%d", dwInstNumber);

			IdeApiSendMessage(EIAMT_RUNTIME_ERROR, szMessage);
			return FALSE;
		}

		DWORD dwNumber = (DWORD)eOperation;
		g_sVirtualProcessor.IP += dwOperationStepMap[dwNumber];
		dwInstNumber++;
	}

	return TRUE;
}

VOID
DeinitializeRuntime()
{
	free(g_bVirtualMemory);
}