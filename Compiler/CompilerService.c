#include "global.h"


static BOOL		g_bIde = FALSE;
static CHAR		g_szFilePath[MAX_PATH] = { 0 };

static
BOOL
ParseArgs(
	INT			nArgc,
	PCHAR		*pszArgv
)
{
	if (nArgc < 1 || nArgc > 2)
	{
		return FALSE;
	}

	strcpy(g_szFilePath, pszArgv[0]);

	if (nArgc == 2 && !strcmp(pszArgv[1], "-IDE"))
	{
		g_bIde = TRUE;
	}

	return TRUE;
}

BOOL
InitializeCompilerService(
	INT			nArgc,
	PCHAR		*pszArgv
)
{
	BOOL bResult = ParseArgs(
		nArgc - 1, pszArgv + 1);
	if (!bResult)
	{
		return FALSE;
	}

	if (g_bIde)
	{	
		bResult = InitializeIdeApi();
		if (!bResult)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL
CompilerServiceStart()
{
	COMPILER hCompiler = NULL;
	BOOL bResult = CreateCompiler(
		&hCompiler, g_szFilePath);
	if (!bResult)
	{
		return FALSE;
	}

	IdeApiSendMessage(EIAMT_INFO, "Начало сборки");
	bResult = CompileProgram(hCompiler);
	if (!bResult)
	{
		CloseCompiler(hCompiler);
		return FALSE;
	}
	IdeApiSendMessage(EIAMT_INFO, "Сборка окончена");

	bResult = SaveBytecodeToFile(hCompiler, "./bytecode.bin");
	if (!bResult)
	{
		CloseCompiler(hCompiler);
		return FALSE;
	}
	IdeApiSendMessage(EIAMT_INFO, "Файл сохранен");

	CloseCompiler(hCompiler);
	return TRUE;
}

BOOL
DeinitializeCompilerService()
{
	if (g_bIde)
	{
		DeinitializeIdeApi();
	}
}