#ifndef VIRTUAL_CONTEXT_H
#define VIRTUAL_CONTEXT_H

#define REGISTERS_COUNT		1 << 2
#define STRING_MAX_LENGTH	256

typedef enum _EProcessorOperation
{
	/** Инициализировать PMC */
	EPO_NEW,
	/** Установить значение */
	EPO_SET,

	/** Инкремент */
	EPO_INC,
	/** Декремент */
	EPO_DEC,
	/** Сложение */
	EPO_ADD,
	/** Разность */
	EPO_SUB,
	/** Произведение */
	EPO_MUL,
	/** Частное */
	EPO_DIV,

	/** Длина строки */
	EPO_LENGTH,
	/** Конкатенация строк */
	EPO_CONCAT,
	/** Подстрока */
	EPO_SUBSTR,

	/** Перемещение по метке */
	EPO_BRANCH,
	/** Логическое условие */
	EPO_IF,
	/** Не равно */
	EPO_NE,
	/** Равно */
	EPO_EQ,
	/** Больше */
	EPO_GT,
	/** Меньше */
	EPO_LT,

	/** Вызов подпрограммы */
	EPO_BSR,
	/** Возврат из процедуры по стеку */
	EPO_RET,

	/** Вывод */
	EPO_PRINT,

	/** Вставить в стек */
	EPO_PUSH,
	/** Вытащить из стека */
	EPO_POP,

	/** Конец программы */
	EPO_END
} EProcessorOperation, *PEProcessorOperation;

/** 
 *	Структура записи стека
 */
typedef struct _SVirtualStack
{
	SList	sEntry;
	BYTE	sCell[0];
} SVirtualStack, *PSVirtualStack;

/**
 * Структура виртуального процессора Parrot
 */
typedef struct _SVirtualProcessor
{
	UINT						I[REGISTERS_COUNT];
	CHAR						S[REGISTERS_COUNT][STRING_MAX_LENGTH];
	DOUBLE						N[REGISTERS_COUNT];
	//SParrotMagicCookie			P[REGISTERS_COUNT];

	UINT64						IP;
} SVirtualProcessor, *PSVirtualProcessor;

/**
 * Виртуальная оперативная память
 */
static BYTE* g_bVirtualMemory = (BYTE*)0;



#endif