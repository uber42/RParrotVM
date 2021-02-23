#ifndef VIRTUAL_CONTEXT_H
#define VIRTUAL_CONTEXT_H

#define REGISTERS_COUNT		1 << 2

typedef enum _EProcessorOperation
{
	/** ���������������� PMC */
	EPO_NEW = 1,
	/** ���������� �������� */
	EPO_SET,

	/** ��������� */
	EPO_INC,
	/** ��������� */
	EPO_DEC,
	/** �������� */
	EPO_ADD_2,
	EPO_ADD_3,
	/** �������� */
	EPO_SUB_2,
	EPO_SUB_3,
	/** ������������ */
	EPO_MUL_2,
	EPO_MUL_3,
	/** ������� */
	EPO_DIV_2,
	EPO_DIV_3,

	/** ����� ������ */
	EPO_LENGTH,
	/** ������������ ����� */
	EPO_CONCAT,
	/** ��������� */
	EPO_SUBSTR,

	/** ����������� �� ����� */
	EPO_BRANCH,
	/** ���������� ������� */
	EPO_IF_2,
	EPO_IF_3,
	/** �� ����� */
	EPO_NE_3,
	EPO_NE_4,
	/** ����� */
	EPO_EQ_3,
	EPO_EQ_4,
	/** ������ */
	EPO_GT_3,
	EPO_GT_4,
	/** ������ */
	EPO_LT_3,
	EPO_LT_4,

	/** ����� ������������ */
	EPO_BSR,
	/** ������� �� ��������� �� ����� */
	EPO_RET,

	/** ����� */
	EPO_PRINT,

	/** �������� � ���� */
	EPO_PUSH_STACK,
	/** �������� �� ����� */
	EPO_POP_STACK,

	/** �������� � PMC */
	EPO_PMC_PUSH,
	/** �������� �� PMC */
	EPO_PMC_POP,
	/** ������� �� PMC */
	EPO_PMC_ERASE,

	/** ����� ��������� */
	EPO_END,

	/** ��� PMC �������� */
	EPO_TYPEOF
} EProcessorOperation, *PEProcessorOperation;

/** 
 *	��������� ������ �����
 */
typedef struct _SVirtualStack
{
	SList	sEntry;
	BYTE	sCell[0];
} SVirtualStack, *PSVirtualStack;

/**
 * ��������� ������������ ���������� Parrot
 */
typedef struct _SVirtualProcessor
{
	UINT						I[REGISTERS_COUNT];
	CHAR						S[REGISTERS_COUNT][STRING_MAX_LENGTH];
	DOUBLE						N[REGISTERS_COUNT];
	SParrotMagicCookie			P[REGISTERS_COUNT];

	UINT64						IP;
} SVirtualProcessor, *PSVirtualProcessor;


#endif