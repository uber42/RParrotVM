#ifndef COMPILER_H
#define COMPILER_H

typedef struct _SBytecode
{
	/** ����� �������� */
	DWORD	nOp : 6;

	/** �������� */
	DWORD	pdwOperators[4];
} SBytecode, *PSBytecode;

#endif