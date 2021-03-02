#ifndef TOKEN_H
#define TOKEN_H

#ifdef __cplusplus
extern "C"
#endif

typedef enum _ETokenType
{
	EMT_I				= 1 << 0,
	EMT_N				= 1 << 1,
	EMT_S				= 1 << 2,
	EMT_P				= 1 << 3,

	EMT_VIRTUAL_MEMORY	= 1 << 4,

	EMT_MARKER			= 1 << 5,

	EMT_STRING_LITERAL	= 1 << 6,
	EMT_INTEGER_LITERAL = 1 << 7,
	EMT_FLOAT_LITERAL	= 1 << 8,

	PMC_INTEGER			= 1 << 9,
	PMC_FLOAT			= 1 << 10,
	PMC_STRING			= 1 << 11,
	PMC_LIST			= 1 << 12,
	PMC_HASHTABLE		= 1 << 13,

	EMT_COMMAND			= ~((WORD)-1)
} ETokenType, *PETokenType;

#define ALL_MEMORY_MASK			0x0000001F
#define REGISTERS_MASK			0x0000000F
#define VIRTUAL_MEMORY_ONLY		0x00000010
#define ALL_LITERAL_MASK		0x000000C0
#define REGISTERS_AND_LITERAL	0x000000CF
#define ALL_NUMBER_MASK			0x0000018B
#define PMC_TYPE_MASK			0x00001F00
#define INTEGERS_MASK			0x00000089
#define NATIVE_REG_MASK			(EMT_I | EMT_N | EMT_S)
#define NATIVE_MASK				(NATIVE_REG_MASK | ALL_LITERAL_MASK)
#define EMT_NUMBER_LITERAL		(EMT_FLOAT_LITERAL | EMT_INTEGER_LITERAL)

#define LINE_MAX_LEXEME_COUNT 5


typedef struct _SLexemeContainer
{
	/** Количество лексем */
	DWORD				dwCount;

	/** Лексемы */
	CHAR				szLexemes[LINE_MAX_LEXEME_COUNT][STRING_MAX_LENGTH];

	/** Токен */
	ETokenType			eToken[LINE_MAX_LEXEME_COUNT];

	/** Выделенная операция на виртуальном процессоре */
	EProcessorOperation eEndPointOperation;
} SLexemeContainer, * PSLexemeContainer;

#endif