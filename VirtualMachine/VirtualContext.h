#ifndef VIRTUAL_CONTEXT_H
#define VIRTUAL_CONTEXT_H

#define REGISTERS_COUNT		1 << 2
#define STRING_MAX_LENGTH	256

typedef enum _EProcessorOperation
{
	/** ���������������� PMC */
	EPO_NEW,
	/** ���������� �������� */
	EPO_SET,

	/** ��������� */
	EPO_INC,
	/** ��������� */
	EPO_DEC,
	/** �������� */
	EPO_ADD,
	/** �������� */
	EPO_SUB,
	/** ������������ */
	EPO_MUL,
	/** ������� */
	EPO_DIV,

	/** ����� ������ */
	EPO_LENGTH,
	/** ������������ ����� */
	EPO_CONCAT,
	/** ��������� */
	EPO_SUBSTR,

	/** ����������� �� ����� */
	EPO_BRANCH,
	/** ���������� ������� */
	EPO_IF,
	/** �� ����� */
	EPO_NE,
	/** ����� */
	EPO_EQ,
	/** ������ */
	EPO_GT,
	/** ������ */
	EPO_LT,

	/** ����� ������������ */
	EPO_BSR,
	/** ������� �� ��������� �� ����� */
	EPO_RET,

	/** ����� */
	EPO_PRINT,

	/** �������� � ���� */
	EPO_PUSH,
	/** �������� �� ����� */
	EPO_POP,

	/** ����� ��������� */
	EPO_END
} EProcessorOperation, *PEProcessorOperation;

/** 
 *	��������� ������ �����
 */
typedef struct _SVirtualStack
{
	SList	sEntry;
	u8		sCell[0];
} SVirtualStack, *PSVirtualStack;

/**
 * ��������� ������������ ���������� Parrot
 */
typedef struct _SVirtualProcessor
{
	u64							I[REGISTERS_COUNT];
	u8							S[REGISTERS_COUNT][STRING_MAX_LENGTH];
	f64							N[REGISTERS_COUNT];
	SParrotMagicCookie			P[REGISTERS_COUNT];

	u64							IP;
} SVirtualProcessor, *PSVirtualProcessor;

/**
 * ����������� ����������� ������
 */
static u8* g_bVirtualMemory = (u8*)0;



#endif