#include "global.h"

/** Операции */
static const DWORD keywords[] =
{
	0x28999611, 0xC6270703, 0xA8E99C47, 0xC25979D3,
	0x3B391274, 0xDC4E3915, 0xEB84ED81, 0xE562AB48,
	0x83D03615, 0xF5CF8C7D, 0x4981C820, 0xB6873945,
	0x39386E06, 0x5836603C, 0x441A6A43, 0x4B208576,
	0x5D31EAED, 0x5EE476F0, 0x30F467AC, 0x16378A88,
	0x876FFFDD, 0x51335FD0, 0x6A8E75AA
};

typedef struct _SPmcMatch
{
	DWORD		dwHash;

	ETokenType	eToken;
} SPmcMatch;

/** Типы PMC */
static const SPmcMatch pmc_types[] =
{
	{ 0x8A8F117F, PMC_INTEGER	},
	{ 0x94AD803F, PMC_FLOAT		},
	{ 0xF4CEE288, PMC_STRING	},
	{ 0xD8E29273, PMC_LIST		},
	{ 0xC98C9CD1, PMC_HASHTABLE	}
};

static 
BOOL 
CheckWhiteSpace(
	CHAR c
)
{
	return c == ' ' || c == ',' || c == '\t';
}

BOOL
PrepareCommand(
	PCHAR				pszCommandLine,
	PSLexemeContainer	psLexemeContainer
)
{
	DWORD dwLength = strlen(pszCommandLine);
	BOOL bNextLexeme = TRUE;
	DWORD dwLastCommandEnd = 0;

	for (DWORD i = 0; i < dwLength; i++)
	{
		CHAR c = pszCommandLine[i];

		/** Пропускаем пробелы и запятые */
		if (CheckWhiteSpace(pszCommandLine[i]))
		{
			if (!psLexemeContainer->dwCount && pszCommandLine[i] == ',')
			{
				return FALSE;
			}
			else if (pszCommandLine[i] == ',' && bNextLexeme)
			{
				return FALSE;
			}
			else if (pszCommandLine[i] == ',')
			{
				bNextLexeme = TRUE;
			}

			continue;
		}

		/** Комментарий */
		if (pszCommandLine[i] == '#')
		{
			return TRUE;
		}

		if (!bNextLexeme && psLexemeContainer->dwCount)
		{
			return FALSE;
		}

		if (i + 1 > dwLength)
		{
			return FALSE;
		}

		DWORD dwCurrentCount = psLexemeContainer->dwCount;

		/** Операция или метка */
		if (!dwCurrentCount)
		{
			DWORD j = i;
			DWORD k = 0;

			for (; j < dwLength && !CheckWhiteSpace(pszCommandLine[j]); j++);
			for (k = j; k < dwLength && 
				(pszCommandLine[k] == '\t' || pszCommandLine[k] == ' '); k++);
			if (pszCommandLine[k] == ',')
			{
				return FALSE;
			}

			/** Валидация */
			if (j - i < 2)
			{
				return FALSE;
			}

			/** Метка */
			if (pszCommandLine[j - 1] == ':')
			{
				for (DWORD k = i; k < j - 1; k++)
				{
					if (!isalpha(pszCommandLine[k]))
					{
						return FALSE;
					}
				}

				memset(psLexemeContainer->szLexemes[dwCurrentCount], 0, STRING_MAX_LENGTH);
				memcpy(psLexemeContainer->szLexemes[dwCurrentCount], pszCommandLine + i, j - i - 1);
				psLexemeContainer->eToken[dwCurrentCount] = EMT_MARKER;
			}
			/** Операция */
			else
			{
				CHAR szTemp[16] = { 0 };
				memcpy(szTemp, pszCommandLine + i, j - i);

				BOOL bFound = FALSE;
				DWORD dwHash = Fnv1aCompute(szTemp, j - i, sizeof(CHAR), FNV_1A_SEED);
				for (DWORD k = 0; k < sizeof(keywords) / sizeof(DWORD); k++)
				{
					if (keywords[k] == dwHash)
					{
						bFound = TRUE;
						break;
					}
				}

				if (!bFound)
				{
					return FALSE;
				}

				memcpy(psLexemeContainer->szLexemes[dwCurrentCount], szTemp, sizeof(szTemp));
				psLexemeContainer->eToken[dwCurrentCount] = EMT_COMMAND;

				bNextLexeme = TRUE;
				i = j - 1;
				psLexemeContainer->dwCount++;
				dwLastCommandEnd = i + 1;

				continue;
			}

			i = j;
		}
		/** Числовые литералы */
		else if (isdigit(pszCommandLine[i]))
		{
			DWORD j = i;
			for (; j < dwLength && !CheckWhiteSpace(pszCommandLine[j]); j++);
			for (DWORD k = i; k < j; k++)
			{
				if (!isdigit(pszCommandLine[k]))
				{
					return FALSE;
				}
			}

			memset(psLexemeContainer->szLexemes[dwCurrentCount], 0, STRING_MAX_LENGTH);
			memcpy(psLexemeContainer->szLexemes[dwCurrentCount], pszCommandLine + i, j - i);
			psLexemeContainer->eToken[dwCurrentCount] = EMT_NUMBER_LITERAL;

			i = j - 1;
		}
		/** Строковые литералы */
		else if (pszCommandLine[i] == '\"')
		{
			DWORD j = i + 1;
			for (; j < dwLength && pszCommandLine[j] != '\"'; j++);
			if (j == dwLength || j - 1 - i < 1)
			{
				return FALSE;
			}

			memset(psLexemeContainer->szLexemes[dwCurrentCount], 0, STRING_MAX_LENGTH);
			memcpy(psLexemeContainer->szLexemes[dwCurrentCount], pszCommandLine + i + 1, j - 1 - i);
			psLexemeContainer->eToken[dwCurrentCount] = EMT_STRING_LITERAL;

			i = j;
		}
		/** Маркеры и регистры */
		else if (isalpha(pszCommandLine[i]))
		{
			DWORD j = i;
			for (; j < dwLength && !CheckWhiteSpace(pszCommandLine[j]); j++);

			/** Маркеры */
			if (j - 1 - i > 2)
			{
				for (DWORD k = i; k < j; k++)
				{
					if (!isalpha(pszCommandLine[k]) && pszCommandLine[k] != '_')
					{
						return FALSE;
					}
				}

				memset(psLexemeContainer->szLexemes[dwCurrentCount], 0, STRING_MAX_LENGTH);
				memcpy(psLexemeContainer->szLexemes[dwCurrentCount], pszCommandLine + i, j - i);
				psLexemeContainer->eToken[dwCurrentCount] = EMT_MARKER;

				i = j - 1;
			}
			/** Целочисленные регистры */
			else if (pszCommandLine[i] == 'I')
			{
				if (pszCommandLine[i + 1] < '0' || pszCommandLine[i + 1] > '3')
				{
					return FALSE;
				}

				if (i + 2 != dwLength && !CheckWhiteSpace(pszCommandLine[i + 2]))
				{
					return FALSE;
				}

				memset(psLexemeContainer->szLexemes[dwCurrentCount], 0, STRING_MAX_LENGTH);
				memcpy(psLexemeContainer->szLexemes[dwCurrentCount], pszCommandLine + i, 2);
				psLexemeContainer->eToken[dwCurrentCount] = EMT_I;

				i++;
			}
			/** Регистры чисел с плавающей запятой */
			else if (pszCommandLine[i] == 'N')
			{
				if (pszCommandLine[i + 1] < '0' || pszCommandLine[i + 1] > '3')
				{
					return FALSE;
				}

				if (i + 2 != dwLength && !CheckWhiteSpace(pszCommandLine[i + 2]))
				{
					return FALSE;
				}

				memset(psLexemeContainer->szLexemes[dwCurrentCount], 0, STRING_MAX_LENGTH);
				memcpy(psLexemeContainer->szLexemes[dwCurrentCount], pszCommandLine + i, 2);
				psLexemeContainer->eToken[dwCurrentCount] = EMT_N;

				i++;
			}
			/** Строковые регистры */
			else if (pszCommandLine[i] == 'S')
			{
				if (pszCommandLine[i + 1] < '0' || pszCommandLine[i + 1] > '3')
				{
					return FALSE;
				}

				if (i + 2 != dwLength && !CheckWhiteSpace(pszCommandLine[i + 2]))
				{
					return FALSE;
				}

				memset(psLexemeContainer->szLexemes[dwCurrentCount], 0, STRING_MAX_LENGTH);
				memcpy(psLexemeContainer->szLexemes[dwCurrentCount], pszCommandLine + i, 2);
				psLexemeContainer->eToken[dwCurrentCount] = EMT_S;

				i++;
			}
			/** PMC регистры */
			else if (pszCommandLine[i] == 'P')
			{
				if (pszCommandLine[i + 1] < '0' || pszCommandLine[i + 1] > '3')
				{
					return FALSE;
				}

				if (i + 2 != dwLength && !CheckWhiteSpace(pszCommandLine[i + 2]))
				{
					return FALSE;
				}

				memset(psLexemeContainer->szLexemes[dwCurrentCount], 0, STRING_MAX_LENGTH);
				memcpy(psLexemeContainer->szLexemes[dwCurrentCount], pszCommandLine + i, 2);
				psLexemeContainer->eToken[dwCurrentCount] = EMT_P;

				i++;
			}
		}
		/** Обращения к виртуальной памяти */
		else if (pszCommandLine[i] == '[')
		{
			DWORD j = i + 1;
			for (; j < dwLength && pszCommandLine[j] != ']'; j++);
			if (j == dwLength || j - 1 - i < 3)
			{
				return FALSE;
			}

			/** Валидация */
			if (pszCommandLine[i + 1] != '0' || pszCommandLine[i + 2] != 'x')
			{
				return FALSE;
			}

			for (DWORD k = i + 3; k < j - 1 - i; k++)
			{
				if (!isxdigit(pszCommandLine[k]))
				{
					return FALSE;
				}
			}

			memset(psLexemeContainer->szLexemes[dwCurrentCount], 0, STRING_MAX_LENGTH);
			memcpy(psLexemeContainer->szLexemes[dwCurrentCount], pszCommandLine + i + 3, j - 1 - i - 2);
			psLexemeContainer->eToken[dwCurrentCount] = EMT_VIRTUAL_MEMORY;

			i = j;
		}
		/** Типы PMC */
		else if (pszCommandLine[i] == '.')
		{
			DWORD j = i + 1;
			for (; j < dwLength && !CheckWhiteSpace(pszCommandLine[j]); j++);
			
			CHAR szTemp[16] = { 0 };
			memcpy(szTemp, pszCommandLine + i + 1, j - i - 1);

			DWORD dwIndex = 0;
			BOOL bFound = FALSE;
			DWORD dwHash = Fnv1aCompute(szTemp, j - i, sizeof(CHAR), FNV_1A_SEED);
			for (DWORD k = 0; k < sizeof(pmc_types) / sizeof(SPmcMatch); k++)
			{
				if (pmc_types[k].dwHash == dwHash)
				{
					dwIndex = k;
					bFound = TRUE;
					break;
				}
			}

			if (!bFound)
			{
				return FALSE;
			}

			memcpy(psLexemeContainer->szLexemes[dwCurrentCount], szTemp, sizeof(szTemp));
			psLexemeContainer->eToken[dwCurrentCount] = pmc_types[dwIndex].eToken;

			i = j;
		}

		dwLastCommandEnd = i + 1;
		bNextLexeme = FALSE;
		psLexemeContainer->dwCount++;
	}

	for (DWORD i = dwLastCommandEnd; i < dwLength; i++)
	{
		if (pszCommandLine[i] != ' ' && pszCommandLine[i] != '\t')
		{
			return FALSE;
		}
	}

	return (psLexemeContainer->dwCount == 1 && bNextLexeme) || !bNextLexeme;
}