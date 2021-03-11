#include "global.h"

static BOOL		g_bIde = FALSE;
static CHAR		g_szFilePath[MAX_PATH] = { 0 };

static
BOOL
ParseArgs(
	INT			nArgc,
	PCHAR* pszArgv
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
InitializeVmService(
	INT			nArgc,
	PCHAR*		pszArgv
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
VmServiceStart()
{
	BOOL bResult = InititalizeRuntime(
		0x140000,
		0x020000,
		0x040000
	);
	if (!bResult)
	{
		return FALSE;
	}
	
	IdeApiSendMessage(EIAMT_INFO, "Загрузка программы в адресное пространство ВМ");
	bResult = LoadProgram(g_szFilePath);
	if (!bResult)
	{
		IdeApiSendMessage(EIAMT_ERROR, "Ошибка открытия файла программы");
		DeinitializeRuntime();
		return FALSE;
	}

	IdeApiSendMessage(EIAMT_INFO, "Запуск программы");
	bResult = StartProgram();
	if (!bResult)
	{
		DeinitializeRuntime();
		return FALSE;
	}

	IdeApiSendMessage(EIAMT_INFO, "Выполнение программы закончено");
	DeinitializeRuntime();

	return TRUE;
}

VOID
DeinitializeVmService()
{
	if (g_bIde)
	{
		DeinitializeIdeApi();
	}
}