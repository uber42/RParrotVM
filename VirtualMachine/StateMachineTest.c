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

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("branch MARKER", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("branch P0", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("substr S0, \"hello world\", 5, 8", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("substr S0, \"hello world\", I0, 8", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("substr S0, S1, I0, 8", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("substr S0, S1, P0, 8", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("substr S0, S1, P0, I2", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("substr S0, S1, S1, I2", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("substr S0, S1, 2, S2", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("substr S0, I1, 2, 6", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("substr I0, S1, 2, 6", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);
}

VOID
StateMachineTestComplexLayers()
{
	SLexemeContainer sContainer;
	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("add I0, I2, 151", &sContainer);
	BOOL bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("add I0, 152, 151", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("add I0, 152", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("add I0, I3", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("set [0x0152], I3", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("set I2, [0x0152]", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("set I2, I3", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("new P0, .Integer", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("push P0, \"New List Item\"", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("push P0, \"New List Item\", 1", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("push P0, \"New List Item\", I0", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("push P0, \"New List Item\", P1", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("push P0, S1, P1", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("push P0", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("push P0, 1, 1", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("push I0", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("pop I0", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("pop I0, P0, S0", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("pop I0, P0, N0", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(!bResult);

	memset(&sContainer, 0, sizeof(SLexemeContainer));
	PrepareCommand("pop N2, P0, S0", &sContainer);
	bResult = StateMachineDriveLexemes(&sContainer);
	assert(bResult);
}