#ifndef TOKEN_H
#define TOKEN_H

typedef enum _ETokenType
{
	EMT_I				= 1 << 0,
	EMT_N				= 1 << 1,
	EMT_S				= 1 << 2,
	EMT_P				= 1 << 3,

	EMT_VIRTUAL_MEMORY	= 1 << 4,

	EMT_MARKER			= 1 << 5,

	EMT_STRING_LITERAL	= 1 << 6,
	EMT_NUMBER_LITERAL  = 1 << 7,

	EMT_PMC_TYPE		= 1 << 8,

	EMT_COMMAND			= ~((WORD)-1)
} ETokenType, *PETokenType;

#define ALL_MEMORY_MASK			0x0000001F
#define REGISTERS_MASK			0x0000000F
#define VIRTUAL_MEMORY_ONLY		0x00000010
#define ALL_LITERAL_MASK		0x000000C0
#define REGISTERS_AND_LITERAL	0x000000CF
#define ALL_NUMBER_MASK			0x0000008B


//static struct
//{
//	EProcessorOperation		eOperation;
//
//	CHAR					szName[16];
//} CommandMatch[]
//{
//	{ EPO_NEW, "new" }
//};


#endif