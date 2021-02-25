#include "global.h"

#pragma pack(push, 1)

typedef struct _SBytecode
{
	/** Операция */
	BYTE	dwOperation;

	/** Данные */
	DWORD	dwOperands[4];
} SBytecode, *PSBytecode;

typedef struct _SBytecodeList
{
	SList		sEntry;

	DWORD		dwCount;

	SBytecode	sBytecode;
} SBytecodeList, *PSBytecodeList;

typedef struct _SByteStringList
{
	SList	sEntry;

	/** Длина строки */
	SIZE_T	nLength;

	/** Текстовый литерал */
	CHAR	szString[STRING_MAX_LENGTH];
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

	/** Байткод */
	SList	sBytecode;
} SCompilerContext, *PSCompilerContext;

#pragma pack(pop)

static
VOID
ChompString(
	PCHAR szLine
) 
{
	PCHAR pChar = strchr(szLine, '\n');

	if (pChar != NULL)
	{
		INT index = (INT)(pChar - szLine);
		szLine[index] = '\0';
	}
}

static
BOOL
HandleLexemes(
	PSCompilerContext	psCompilerContext,
	PSLexemeContainer   psLexemeContainer,
	PDWORD				pdwCurrentOffset
)
{
	if (psLexemeContainer->eToken[0] == EMT_MARKER)
	{
		if (psLexemeContainer->dwCount != 1)
		{
			return FALSE;
		}

		PSByteMarkerList psMarkerEntry = malloc(sizeof(SByteMarkerList));
		if (!psMarkerEntry)
		{
			return FALSE;
		}

		strcpy(psMarkerEntry->szMarker, psLexemeContainer->szLexemes[0]);
		psMarkerEntry->dwOffset = *pdwCurrentOffset;

		ListAddToEnd(&psMarkerEntry->sEntry, &psCompilerContext->sMarkersList);
		return TRUE;
	}

	BOOL bResult = StateMachineDriveLexemes(psLexemeContainer);
	if (!bResult)
	{
		return FALSE;
	}

	PSLexemeList psLexemeEntry = malloc(sizeof(SLexemeList));
	if (!psLexemeEntry)
	{
		return FALSE;
	}

	memcpy(&psLexemeEntry->sLexeme, psLexemeContainer, sizeof(SLexemeContainer));
	ListAddToEnd(&psLexemeEntry->sEntry, &psCompilerContext->sLexemeList);

	psCompilerContext->dwLexemeCount++;
	*pdwCurrentOffset += sizeof(BYTE) + sizeof(DWORD) * (psLexemeContainer->dwCount - 1);
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
	DWORD dwOffset = 0;
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
		if (sLexemeContainer.dwCount == 0)
		{
			continue;
		}

		bResult = HandleLexemes(psCompilerContext, &sLexemeContainer, &dwOffset);
		if (!bResult)
		{
			bSuccess = FALSE;
			break;
		}
	}

	 fclose(pFile);
	 return bSuccess;
}

static
BOOL
CompileAssembler(
	PSCompilerContext	psCompilerContext
)
{
	for (PSList psCurrentEntry = psCompilerContext->sLexemeList.pFlink;
		psCurrentEntry != &psCompilerContext->sLexemeList;
		psCurrentEntry = psCurrentEntry->pFlink)
	{
		PSBytecodeList psCurrentBytecode = malloc(sizeof(SBytecodeList));
		if (!psCurrentBytecode)
		{
			return FALSE;
		}
		memset(psCurrentBytecode, 0, sizeof(SBytecode));

		PSLexemeList psContainingEntry = CONTAINING_RECORD(
			psCurrentEntry, SLexemeList, sEntry);

		psCurrentBytecode->sBytecode.dwOperation = psContainingEntry->sLexeme.eEndPointOperation;
		psCurrentBytecode->dwCount = psContainingEntry->sLexeme.dwCount;
		for (DWORD i = 1; i < psContainingEntry->sLexeme.dwCount; i++)
		{
			if (psContainingEntry->sLexeme.eToken[i] == EMT_STRING_LITERAL)
			{
				PSByteStringList psByteString = malloc(sizeof(SByteStringList));
				if (!psByteString)
				{
					free(psCurrentBytecode);
					return FALSE;
				}
				memset(psByteString, 0, sizeof(SByteStringList));
				strcpy(psByteString->szString, psContainingEntry->sLexeme.szLexemes[i]);
				psByteString->nLength = strlen(psContainingEntry->sLexeme.szLexemes[i]);

				psCurrentBytecode->sBytecode.dwOperands[i - 1] = psCompilerContext->dwStringLiteralCount;
				psCurrentBytecode->sBytecode.dwOperands[i - 1] |= BYTECODE_STRING_LITERAL_FLAG;
				
				psCompilerContext->dwStringLiteralCount++;

				ListAddToEnd(&psByteString->sEntry, &psCompilerContext->sStringList);
			}
			else if(psContainingEntry->sLexeme.eToken[i] == EMT_MARKER)
			{
				DWORD dwOffset = -1;
				for (PSList psCurrentMarker = psCompilerContext->sMarkersList.pFlink;
					psCurrentMarker != &psCompilerContext->sMarkersList;
					psCurrentMarker = psCurrentMarker->pFlink)
				{
					PSByteMarkerList psMarker = CONTAINING_RECORD(
						psCurrentMarker, SByteMarkerList, sEntry);
					if (!strcmp(psMarker->szMarker, psContainingEntry->sLexeme.szLexemes[i]))
					{
						dwOffset = psMarker->dwOffset;
						break;
					}
				}

				if (dwOffset == (DWORD)-1)
				{
					free(psCurrentBytecode);
					return FALSE;
				}

				psCurrentBytecode->sBytecode.dwOperands[i - 1] = dwOffset;
			}
			else if((psContainingEntry->sLexeme.eToken[i] & REGISTERS_MASK) == psContainingEntry->sLexeme.eToken[i])
			{
				DWORD dwRegisterNumber = ((int)psContainingEntry->sLexeme.szLexemes[i][1] - 48);

				psCurrentBytecode->sBytecode.dwOperands[i - 1] = psContainingEntry->sLexeme.eToken[i];
				psCurrentBytecode->sBytecode.dwOperands[i - 1] |= BYTECODE_REGISTER_FLAG;
				psCurrentBytecode->sBytecode.dwOperands[i - 1] |= (dwRegisterNumber << 25);
			}
			else if ((psContainingEntry->sLexeme.eToken[i] & EMT_VIRTUAL_MEMORY) == psContainingEntry->sLexeme.eToken[i])
			{
				psCurrentBytecode->sBytecode.dwOperands[i - 1] = strtol(psContainingEntry->sLexeme.szLexemes[i], NULL, 16);
				psCurrentBytecode->sBytecode.dwOperands[i - 1] |= BYTECODE_VIRTUAL_MEMORY_FLAG;
			}
			else if ((psContainingEntry->sLexeme.eToken[i] & EMT_NUMBER_LITERAL) == psContainingEntry->sLexeme.eToken[i])
			{
				psCurrentBytecode->sBytecode.dwOperands[i - 1] = strtol(psContainingEntry->sLexeme.szLexemes[i], NULL, 10);
				psCurrentBytecode->sBytecode.dwOperands[i - 1] |= BYTECODE_NUMBER_LITERAL_FLAG;
			}
			else
			{
				psCurrentBytecode->sBytecode.dwOperands[i - 1] = psContainingEntry->sLexeme.eToken[i];
			}
		}

		ListAddToEnd(&psCurrentBytecode->sEntry, &psCompilerContext->sBytecode);
	}

	return TRUE;
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

	strcpy(psCompilerContext->szFileName, szFileName);

	ListHeadInit(&psCompilerContext->sLexemeList);
	ListHeadInit(&psCompilerContext->sMarkersList);
	ListHeadInit(&psCompilerContext->sStringList);
	ListHeadInit(&psCompilerContext->sBytecode);

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

	bResult = CompileAssembler(psCompilerContext);
	if (!bResult)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL
SaveBytecodeToFile(
	COMPILER	hCompiler,
	CHAR		szFileName[MAX_PATH]
)
{
	PSCompilerContext psCompilerContext = hCompiler;
	FILE* pFile = fopen(szFileName, "wb");
	if (pFile == NULL)
	{
		return FALSE;
	}

	SFrozenFileHeader sHeader = { 0 };
	sHeader.dwLiteralsCount = psCompilerContext->dwStringLiteralCount;

	fseek(pFile, sizeof(SFrozenFileHeader), SEEK_SET);

	DWORD dwOffset = sizeof(SFrozenFileHeader) + psCompilerContext->dwStringLiteralCount * sizeof(DWORD);
	for (PSList psCurrentEntry = psCompilerContext->sStringList.pFlink;
		psCurrentEntry != &psCompilerContext->sStringList;
		psCurrentEntry = psCurrentEntry->pFlink)
	{
		PSByteStringList psString = CONTAINING_RECORD(
			psCurrentEntry, SByteStringList, sEntry);
		SIZE_T nCountWrote = fwrite(&dwOffset, sizeof(DWORD), 1, pFile);
		if (!nCountWrote)
		{
			fclose(pFile);
			return FALSE;
		}
		dwOffset += psString->nLength + 1;
	}

	for (PSList psCurrentEntry = psCompilerContext->sStringList.pFlink;
		psCurrentEntry != &psCompilerContext->sStringList;
		psCurrentEntry = psCurrentEntry->pFlink)
	{
		PSByteStringList psString = CONTAINING_RECORD(
			psCurrentEntry, SByteStringList, sEntry);
		SIZE_T nCountWrote = fwrite(psString->szString, psString->nLength + 1, 1, pFile);
		if (!nCountWrote)
		{
			fclose(pFile);
			return FALSE;
		}
	}

	for (PSList psCurrentEntry = psCompilerContext->sBytecode.pFlink;
		psCurrentEntry != &psCompilerContext->sBytecode;
		psCurrentEntry = psCurrentEntry->pFlink)
	{
		PSBytecodeList psBytecode = CONTAINING_RECORD(
			psCurrentEntry, SBytecodeList, sEntry);

		SIZE_T nSize = sizeof(SBytecode) - (4 - (psBytecode->dwCount - 1)) * sizeof(DWORD);
		SIZE_T nCountWrote = fwrite(&psBytecode->sBytecode, nSize, 1, pFile);
		if (!nCountWrote)
		{
			fclose(pFile);
			return FALSE;
		}
	}

	sHeader.dwCodeMapOffset = dwOffset;
	fseek(pFile, 0, SEEK_SET);
	SIZE_T nCountWrote = fwrite(&sHeader, sizeof(SFrozenFileHeader), 1, pFile);
	if (!nCountWrote)
	{
		fclose(pFile);
		return FALSE;
	}

	fclose(pFile);
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
	FreeList(&psCompilerContext->sBytecode);

	free(psCompilerContext);
}