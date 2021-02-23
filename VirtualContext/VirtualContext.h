#ifndef VIRTUAL_CONTEXT_H
#define VIRTUAL_CONTEXT_H

#define REGISTERS_COUNT		1 << 2

typedef enum _EProcessorOperation
{
	/** Инициализировать PMC */
	EPO_NEW = 1,
	/** Установить значение */
	EPO_SET,

	/** Инкремент */
	EPO_INC,
	/** Декремент */
	EPO_DEC,
	/** Сложение */
	EPO_ADD_2,
	EPO_ADD_3,
	/** Разность */
	EPO_SUB_2,
	EPO_SUB_3,
	/** Произведение */
	EPO_MUL_2,
	EPO_MUL_3,
	/** Частное */
	EPO_DIV_2,
	EPO_DIV_3,

	/** Длина строки */
	EPO_LENGTH,
	/** Конкатенация строк */
	EPO_CONCAT,
	/** Подстрока */
	EPO_SUBSTR,

	/** Перемещение по метке */
	EPO_BRANCH,
	/** Логическое условие */
	EPO_IF_2,
	EPO_IF_3,
	/** Не равно */
	EPO_NE_3,
	EPO_NE_4,
	/** Равно */
	EPO_EQ_3,
	EPO_EQ_4,
	/** Больше */
	EPO_GT_3,
	EPO_GT_4,
	/** Меньше */
	EPO_LT_3,
	EPO_LT_4,

	/** Вызов подпрограммы */
	EPO_BSR,
	/** Возврат из процедуры по стеку */
	EPO_RET,

	/** Вывод */
	EPO_PRINT,

	/** Вставить в стек */
	EPO_PUSH_STACK,
	/** Вытащить из стека */
	EPO_POP_STACK,

	/** Вставить в PMC */
	EPO_PMC_PUSH,
	/** Вытащить из PMC */
	EPO_PMC_POP,
	/** Удалить из PMC */
	EPO_PMC_ERASE,

	/** Конец программы */
	EPO_END,

	/** Тип PMC регистра */
	EPO_TYPEOF
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
	SParrotMagicCookie			P[REGISTERS_COUNT];

	UINT64						IP;
} SVirtualProcessor, *PSVirtualProcessor;


#endif