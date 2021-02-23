#include "global.h"




int main()
{
	LexerCommandsTest();

	InititalizeStateMachineTables();
	StateMachineTestParsedLexems();
	StateMachineTestComplexLayers();

	//system("PAUSE");
}