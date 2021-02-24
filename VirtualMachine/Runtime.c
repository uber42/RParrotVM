#include "global.h"

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

	g_sRuntimeContext.pHeap = g_bVirtualMemory + nFileSize;
	g_sRuntimeContext.pIndexTable = g_sRuntimeContext.pCodeMap + sizeof(SFrozenFileHeader);

	SFrozenFileHeader sHeader = { 0 };
	memcpy(&sHeader, g_sRuntimeContext.pCodeMap, sizeof(SFrozenFileHeader));

	g_sRuntimeContext.pInstruction = g_sRuntimeContext.pCodeMap + sHeader.dwCodeMapOffset;

	return TRUE;
}

BOOL
StartProgram()
{
	for (; g_sRuntimeContext.pInstruction + g_sVirtualProcessor.IP < g_sRuntimeContext.pHeap;
		g_sVirtualProcessor.IP += sizeof(BYTE))
	{
		PBYTE pCurrentInstruction = g_sRuntimeContext.pInstruction + g_sVirtualProcessor.IP;
		EProcessorOperation eOperation = 0;
		memcpy(&eOperation, pCurrentInstruction, sizeof(BYTE));

		DWORD dwResult = -1;
		DWORD dwError = 0;

		switch (eOperation)
		{
		case EPO_NEW:
			break;
		case EPO_SET:
			dwResult = PasmSet(pCurrentInstruction, &dwError);
			break;
		case EPO_INC:
			break;
		case EPO_DEC:
			break;
		case EPO_ADD_2:
			break;
		case EPO_ADD_3:
			break;
		case EPO_SUB_2:
			break;
		case EPO_SUB_3:
			break;
		case EPO_MUL_2:
			break;
		case EPO_MUL_3:
			break;
		case EPO_DIV_2:
			break;
		case EPO_DIV_3:
			break;
		case EPO_LENGTH:
			break;
		case EPO_CONCAT:
			break;
		case EPO_SUBSTR:
			break;
		case EPO_BRANCH:
			break;
		case EPO_IF_2:
			break;
		case EPO_IF_3:
			break;
		case EPO_NE_3:
			break;
		case EPO_NE_4:
			break;
		case EPO_EQ_3:
			break;
		case EPO_EQ_4:
			break;
		case EPO_GT_3:
			break;
		case EPO_GT_4:
			break;
		case EPO_LT_3:
			break;
		case EPO_LT_4:
			break;
		case EPO_BSR:
			break;
		case EPO_RET:
			break;
		case EPO_PRINT:
			break;
		case EPO_PUSH_STACK:
			break;
		case EPO_POP_STACK:
			break;
		case EPO_PMC_PUSH:
			break;
		case EPO_PMC_POP:
			break;
		case EPO_PMC_ERASE:
			break;
		case EPO_END:
			break;
		case EPO_TYPEOF:
			break;
		}

		if (dwResult == (DWORD)-1)
		{
			// HandleError(dwError);
			return FALSE;
		}
	}

	return TRUE;
}

VOID
DeinitializeRuntime()
{
	free(g_bVirtualMemory);
}