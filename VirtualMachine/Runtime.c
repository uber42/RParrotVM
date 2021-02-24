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

VOID
DeinitializeRuntime()
{
	free(g_bVirtualMemory);
}