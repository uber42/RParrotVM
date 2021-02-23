#include "global.h"

VOID
LexerCommandsTest()
{
	SLexemeContainer sContainer = { 0 };

	BOOL bResult = PrepareCommand("set P0, I1", &sContainer);
	assert(bResult);
	assert(sContainer.dwCount == 3);
	assert(sContainer.eToken[0] == EMT_COMMAND);
	assert(sContainer.eToken[1] == EMT_P);
	assert(sContainer.eToken[2] == EMT_I);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("print \"Hello World\"", &sContainer);
	assert(bResult);
	assert(sContainer.dwCount == 2);
	assert(sContainer.eToken[0] == EMT_COMMAND);
	assert(sContainer.eToken[1] == EMT_STRING_LITERAL);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set P0 ,I1", &sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand(",set P0 ,I1", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set, P0 ,I1", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set P0 , I1", &sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set P0,, I1", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set P0 I1", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set P4 ,I1", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set P, I1", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set 0, I1", &sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set [0xFFFF2020], I1", &sContainer);
	assert(bResult);
	assert(sContainer.dwCount == 3);
	assert(sContainer.eToken[0] == EMT_COMMAND);
	assert(sContainer.eToken[1] == EMT_VIRTUAL_MEMORY);
	assert(sContainer.eToken[2] == EMT_I);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("ret", &sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("push I0,", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("push I0 #,", &sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand(" MARKER: ", &sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("new P0, .Integer", &sContainer);
	assert(bResult);
	assert(sContainer.dwCount == 3);
	assert(sContainer.eToken[0] == EMT_COMMAND);
	assert(sContainer.eToken[1] == EMT_P);
	assert(sContainer.eToken[2] == PMC_INTEGER);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("new P3, .Hashtable", &sContainer);
	assert(bResult);
	assert(sContainer.dwCount == 3);
	assert(sContainer.eToken[0] == EMT_COMMAND);
	assert(sContainer.eToken[1] == EMT_P);
	assert(sContainer.eToken[2] == PMC_HASHTABLE);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("if I0, MARKER_FIRST, MARKER_SECOND", &sContainer);
	assert(bResult);
	assert(sContainer.dwCount == 4);
	assert(sContainer.eToken[0] == EMT_COMMAND);
	assert(sContainer.eToken[1] == EMT_I);
	assert(sContainer.eToken[2] == EMT_MARKER);
	assert(sContainer.eToken[3] == EMT_MARKER);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("lt I0, 556, MARKER_FIRST, MARKER_SECOND", &sContainer);
	assert(bResult);
	assert(sContainer.dwCount == 5);
	assert(sContainer.eToken[0] == EMT_COMMAND);
	assert(sContainer.eToken[1] == EMT_I);
	assert(sContainer.eToken[2] == EMT_NUMBER_LITERAL);
	assert(sContainer.eToken[3] == EMT_MARKER);
	assert(sContainer.eToken[4] == EMT_MARKER);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("push P0, 1", &sContainer);
	assert(bResult);
	assert(sContainer.dwCount == 3);
	assert(sContainer.eToken[0] == EMT_COMMAND);
	assert(sContainer.eToken[1] == EMT_P);
	assert(sContainer.eToken[2] == EMT_NUMBER_LITERAL);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set N2, 3.14", &sContainer);
	assert(bResult);
	assert(sContainer.dwCount == 3);
	assert(sContainer.eToken[0] == EMT_COMMAND);
	assert(sContainer.eToken[1] == EMT_N);
	assert(sContainer.eToken[2] == EMT_NUMBER_LITERAL);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set N2, .314", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set N2, 314.", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set N2, 3.", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set N2, .", &sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	bResult = PrepareCommand("set N2, .3", &sContainer);
	assert(!bResult);
}