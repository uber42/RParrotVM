#ifndef VIRTUAL_CONTEXT_H
#define VIRTUAL_CONTEXT_H

#define REGISTERS_COUNT		1 << 2
#define STRING_MAX_LENGTH	256

typedef struct _SVirtualStack
{
	SList	sEntry;
	u8		sCell[0];
} SVirtualStack, *PSVirtualStack;

typedef struct _SVirtualProcessor
{
	u64							I[REGISTERS_COUNT];
	u8							S[REGISTERS_COUNT][STRING_MAX_LENGTH];
	f64							N[REGISTERS_COUNT];
	//SParrotMagicCookie	D[REGISTERS_COUNT];

	u64					RIP;
} SVirtualProcessor, *PSVirtualProcessor;

/**
 * Виртуальная оперативная память
 */
static u8* g_bVirtualMemory = (u8*)0;



#endif