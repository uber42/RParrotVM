#ifndef TOKEN_H
#define TOKEN_H

typedef enum _EToken
{
	ET_INTEGER			= 1 << 0,
	ET_FLOAT			= 1 << 1,
	ET_STRING			= 1 << 2
} EToken, *PEToken;

typedef enum _EMemoryType
{
	EMT_I				= 1 << 0,
	EMT_N				= 1 << 1,
	EMT_S				= 1 << 2,
	EMT_P				= 1 << 3,

	EMT_VIRTUAL_MEMORY	= 1 << 4,

	EMT_MARKER			= 1 << 5,

	EMT_LITERAL			= 1 << 6
} EMemoryType, *PEMemoryType;

#define ALL_MEMORY_MASK			0x0000001F
#define REGISTERS_MASK			0x0000000F
#define VIRTUAL_MEMORY_ONLY		0x00000010
#define REGISTERS_AND_LITERAL	0x00000079


#endif