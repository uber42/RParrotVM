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
}