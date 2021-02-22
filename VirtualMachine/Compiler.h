#ifndef COMPILER_H
#define COMPILER_H

typedef struct _SBytecode
{
	/** Номер операции */
	DWORD	nOp : 6;

	/** Операнды */
	DWORD	pdwOperators[4];
} SBytecode, *PSBytecode;

#endif