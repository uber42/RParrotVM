#ifndef OPERATIONS_H
#define OPERATIONS_H

BOOL
PasmNew(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmSet(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				psHeap,
	PBYTE				psIndexTable,
	PBYTE				pCurrentInstruction
);

BOOL
PasmInc(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmDec(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmAdd3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmMul3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmDiv3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmSub3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmAdd2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmSub2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmMul2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmDiv2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmLength(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				psIndexTable
);

BOOL
PasmConcat(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				psIndexTable
);

BOOL
PasmSubstr(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				psIndexTable
);

BOOL
PasmPrint(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				psIndexTable
);

BOOL
PasmBranch(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmIf2(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmIf3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmGt3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmLt3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmEq3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmNe3(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);


BOOL
PasmGt4(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmLt4(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmEq4(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmNe4(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmBsr(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pStack
);

BOOL
PasmRet(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pStack
);

BOOL
PasmPushStack(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pStack
);

BOOL
PasmPopStack(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pStack
);

BOOL
PasmPushHashTable(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pIndexTable,
	PBYTE				pVirtualMemory
);

BOOL
PasmPopHashTable(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pIndexTable,
	PBYTE				pVirtualMemory
);

BOOL
PasmEraseHashTable(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				pIndexTable
);

BOOL
PasmChk(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);

BOOL
PasmTypeof(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction
);


#endif
