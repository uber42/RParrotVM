#ifndef LEXER_H
#define LEXER_H

#define LINE_MAX_LEXEME_COUNT 5


typedef struct _SLexemeContainer
{
	/** ���������� ������ */
	DWORD		dwCount;

	/** ������� */
	CHAR		szLexemes[LINE_MAX_LEXEME_COUNT][STRING_MAX_LENGTH];

	/** ����� */
	ETokenType  eToken[LINE_MAX_LEXEME_COUNT];
} SLexemeContainer, *PSLexemeContainer;


BOOL
PrepareCommand(
	PCHAR				pszCommandLine,
	PSLexemeContainer	psLexemeContainer
);

#endif