#include "global.h"

typedef struct _SBytecode
{
	/** Операция */
	BYTE	dwOperation;

	/** Данные */
	DWORD	dwOperands[4];
} SBytecode, *PSBytecode;

typedef struct _SByteStringList
{
	SList	sEntry;

	/** Текстовый литерал */
	CHAR	szString[STRING_MAX_LENGTH];

	/** Номер строки в базе */
	DWORD	dwNumber;
} SByteStringList, *PSByteStringList;

typedef struct _SByteMarkerList
{
	SList	sEntry;

	/** Маркер */
	CHAR	szMarker[STRING_MAX_LENGTH];

	/** Смещение маркера */
	DWORD	dwOffset;
} SByteMarkerList, *PSByteMarkerList;

typedef struct _SLexemeList
{
	SList				sEntry;

	/** Лексемы в рамках одной операции */
	SLexemeContainer	sLexeme;
} SLexemeList, *PSLexemeList;

typedef struct _SCompilerContext
{
	/** Файл с программой */
	CHAR	szFileName[MAX_PATH];

	/** Количество строковых литераолв */
	DWORD	dwStringLiteralCount;

	/** Список меток */
	SList	sMarkersList;

	/** Список литералов строк */
	SList	sStringList;

	/** Количество лексем */
	DWORD	dwLexemeCount;

	/** Список лексем */
	SList	sLexemeList;
} SCompilerContext, *PSCompilerContext;

static
VOID
ChompString(
	PCHAR szLine
) 
{
	BOOL bSuccess = FALSE;
	PCHAR pChar = strchr(szLine, '\n');

	if (pChar != NULL)
	{
		INT index = (INT)(pChar - szLine);
		szLine[index] = '\0';
		bSuccess = TRUE;
	}

	return bSuccess;
}

static
BOOL
HandleLexemes(
	PSCompilerContext	psCompilerContext,
	PSLexemeContainer   psLexemeContainer
)
{
	BOOL bResult = StateMachineDriveLexemes(psLexemeContainer);
	if (!bResult)
	{
		return FALSE;
	}

	if (psLexemeContainer->eToken[0] == EMT_MARKER)
	{
		PSByteMarkerList psMarkerEntry = malloc(sizeof(SByteMarkerList));
		if (!psMarkerEntry)
		{
			return FALSE;
		}

		strcpy(psMarkerEntry->szMarker, psLexemeContainer->szLexemes[0]);
		psMarkerEntry->dwOffset = sizeof(SBytecode) * psCompilerContext->dwLexemeCount;

		ListAddToEnd(psMarkerEntry, &psCompilerContext->sMarkersList);
		return TRUE;
	}

	PSLexemeList psLexemeEntry = malloc(sizeof(SLexemeList));
	if (!psLexemeEntry)
	{
		return FALSE;
	}

	memcpy(&psLexemeEntry->sLexeme, psLexemeContainer, sizeof(SLexemeContainer));
	ListAddToEnd(&psLexemeEntry->sEntry, &psCompilerContext->sLexemeList);

	psCompilerContext->dwLexemeCount++;
	return TRUE;
}

static
BOOL
ReadAssemblerLines(
	PSCompilerContext	psCompilerContext
)
{
	CHAR szBuffer[STRING_MAX_LENGTH * 2];
	FILE* pFile = fopen(psCompilerContext->szFileName, "r");
	if (pFile == NULL)
	{
		return FALSE;
	}

	BOOL bSuccess = TRUE;
	while (fgets(szBuffer, sizeof(szBuffer), pFile) != NULL)
	{
		ChompString(szBuffer);
		SLexemeContainer sLexemeContainer = { 0 };
		BOOL bResult = PrepareCommand(szBuffer, &sLexemeContainer);
		if (!bResult)
		{
			bSuccess = FALSE;
			break;
		}

		bResult = HandleLexemes(psCompilerContext, &sLexemeContainer);
		if (!bResult)
		{
			bSuccess = FALSE;
			break;
		}
	}

	 fclose(pFile);
	 return TRUE;
}

static
BOOL
CompileLine(
	PSLexemeContainer psContainer
)
{

}

BOOL
CreateCompiler(
	PCOMPILER	hCompilerContext,
	CHAR		szFileName[MAX_PATH]
)
{
	assert(hCompilerContext);

	PSCompilerContext psCompilerContext = malloc(sizeof(SCompilerContext));
	if (!psCompilerContext)
	{
		return FALSE;
	}

	strcpy(psCompilerContext->szFileName, szFileName, MAX_PATH);

	ListHeadInit(&psCompilerContext->sLexemeList);
	ListHeadInit(&psCompilerContext->sMarkersList);
	ListHeadInit(&psCompilerContext->sStringList);

	InititalizeStateMachineTables();

	psCompilerContext->dwStringLiteralCount = 0;
	psCompilerContext->dwLexemeCount = 0;

	*hCompilerContext = psCompilerContext;

	return TRUE;
}

BOOL
CompileProgram(
	COMPILER	hCompilerContext
)
{
	PSCompilerContext psCompilerContext = hCompilerContext;

	BOOL bResult = ReadAssemblerLines(psCompilerContext);
	if (!bResult)
	{
		return FALSE;
	}

	return TRUE;
}

static
VOID
FreeList(
	PSList psHead
)
{
	while (!ListIsEmpty(psHead))
	{
		PSList psCurrentEntry = psHead->pFlink;
		ListNodeDelete(psHead->pFlink);

		free(psCurrentEntry);
	}
}

VOID
CloseCompiler(
	COMPILER	hCompiler
)
{
	PSCompilerContext psCompilerContext = hCompiler;
	FreeList(&psCompilerContext->sLexemeList);
	FreeList(&psCompilerContext->sStringList);
	FreeList(&psCompilerContext->sMarkersList);

	free(psCompilerContext);
}