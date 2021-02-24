#ifndef RUNTIME_H
#define RUNTIME_H

typedef struct _SRuntimeContext
{
	/** Размер виртуальной памяти */
	DWORD	dwMemorySize;

	/** Размер стека */
	DWORD	dwStackSize;

	/** Размер кучи */
	DWORD	dwHeapSize;

	/** Указатель на начало стека в памяти */
	PBYTE	pStack;

	/** Указатель на начало инструкций в памяти */
	PBYTE	pCodeMap;

	/** Указатель на таблицу индексов статических данных в памяти */
	PBYTE	pIndexTable;

	/** Указатель начало инструкций */
	PBYTE	pInstruction;

	/** Указатель на начало кучи */
	PBYTE	pHeap;
} SRuntimeContext, *PSRuntimeContext;

BOOL
InititalizeRuntime(
	DWORD	dwMemorySize,
	DWORD	dwStackSize,
	DWORD	dwHeapSize
);

BOOL
LoadProgram(
	CHAR	szFileName[MAX_PATH]
);

BOOL
StartProgram();

VOID
DeinitializeRuntime();

#endif