#include "global.h"



int main()
{
	BOOL bResult = InititalizeRuntime(
		0x140000,
		0x020000,
		0x040000
	);
	if (!bResult)
	{
		ExitProcess(-1);
	}

	bResult = LoadProgram("../Compiler/Bytecode.bin");
	if (!bResult)
	{
		DeinitializeRuntime();
		ExitProcess(-1);
	}

	bResult = StartProgram();
	if (!bResult)
	{
		DeinitializeRuntime();
		ExitProcess(-1);
	}

	DeinitializeRuntime();

	system("PAUSE");
}