#ifndef RUNTIME_H
#define RUNTIME_H

typedef struct _SRuntimeContext
{
	/** ������ ����������� ������ */
	DWORD	dwMemorySize;

	/** ������ ����� */
	DWORD	dwStackSize;

	/** ������ ���� */
	DWORD	dwHeapSize;

	/** ��������� �� ������ ����� � ������ */
	PBYTE	pStack;

	/** ��������� �� ������ ���������� � ������ */
	PBYTE	pCodeMap;

	/** ��������� �� ������� �������� ����������� ������ � ������ */
	PBYTE	pIndexTable;

	/** ��������� ������ ���������� */
	PBYTE	pInstruction;

	/** ��������� �� ������ ���� */
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

VOID
DeinitializeRuntime();

#endif