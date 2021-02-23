#include "global.h"

typedef struct _SBytecode
{
	/** Операция */
	BYTE	dwOperation : 6;

	/** Данные */
	BYTE	pOperands[0];
};

typedef struct _SByteStringList
{
	SList	sEntry;

	/** Текстовый литерал */
	CHAR	szString[STRING_MAX_LENGTH];

	/** Номер строки в базе */
	DWORD	dwNumber;
};

typedef struct _SByteMarkerList
{
	SList	sEntry;

	/** Маркер */
	CHAR	szMarker[STRING_MAX_LENGTH];

	/** Смещение маркера */
	DWORD	dwOffset;
};

typedef struct _SLexemeList
{
	SList				sEntry;

	/** Лексемы в рамках одной операции */
	SLexemeContainer	sLexeme;
};

typedef struct _SCompilerContext
{
	/** Список меток */
	SList	sMarkersList;

	/** Список литералов строк */
	SList	sStringList;
};

static
BOOL
ReadAssemblerLine(
	PSLexemeContainer psContainer
)
{

}

static
BOOL
CompileLine(
	PSLexemeContainer psContainer
)
{

}

BOOL
CompileProgram()
{

}