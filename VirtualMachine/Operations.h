#ifndef OPERATIONS_H
#define OPERATIONS_H

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
PasmPrint(
	PSVirtualProcessor	psVirtualProcessor,
	PBYTE				pCurrentInstruction,
	PBYTE				psIndexTable
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


#endif
