#ifndef VIRTUAL_CONTEXT_H
#define VIRTUAL_CONTEXT_H



/**
 * Структура виртуального процессора Parrot
 */
typedef struct _SVirtualProcessor
{
	/** Регистры общего назначения */
	INT						I[REGISTERS_COUNT];
	CHAR					S[REGISTERS_COUNT][STRING_MAX_LENGTH];
	FLOAT					N[REGISTERS_COUNT];
	/** Parrot Magic Cookie регистр */
	SParrotMagicCookie		P[REGISTERS_COUNT];

	/** Регистр инструкций */
	DWORD					IP;
	/** Регистр указатель стека */
	DWORD					SP;
} SVirtualProcessor, *PSVirtualProcessor;

/**
 * Заголовок фалйа программы
 */
typedef struct _SFrozenFileHeader
{
	/** Количество литералов */
	DWORD	dwLiteralsCount;

	/** Смещение к коду */
	DWORD	dwCodeMapOffset;
} SFrozenFileHeader;

#endif