#include "global.h"

typedef struct _SBytecode
{
	/** �������� */
	BYTE	dwOperation : 6;

	/** ������ */
	BYTE	pOperands[0];
};

typedef struct _SByteStringList
{
	SList	sEntry;

	/** ��������� ������� */
	CHAR	szString[STRING_MAX_LENGTH];

	/** ����� ������ � ���� */
	DWORD	dwNumber;
};

typedef struct _SByteMarkerList
{
	SList	sEntry;

	/** ������ */
	CHAR	szMarker[STRING_MAX_LENGTH];

	/** �������� ������� */
	DWORD	dwOffset;
};

typedef struct _SLexemeList
{
	SList				sEntry;

	/** ������� � ������ ����� �������� */
	SLexemeContainer	sLexeme;
};

typedef struct _SCompilerContext
{
	/** ������ ����� */
	SList	sMarkersList;

	/** ������ ��������� ����� */
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