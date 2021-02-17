#include "global.h"


VOID
StateMachineTestParsedLexems()
{
	SLexemeContainer sContainer;
	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("if I0, MARKER_FIRST, MARKER_SECOND", &sContainer);	
	BOOL bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("if I0, I1, MARKER_SECOND", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);
}