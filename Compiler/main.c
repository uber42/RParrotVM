#include "global.h"




int main()
{
	COMPILER hCompiler = NULL;
	BOOL bResult = CreateCompiler(
		&hCompiler, ".\\Examples\\hashtable.pasm");
	if (!bResult)
	{
		ExitProcess(-1);
	}

	bResult = CompileProgram(hCompiler);
	if (!bResult)
	{
		CloseCompiler(hCompiler);
		ExitProcess(-1);
	}

	bResult = SaveBytecodeToFile(hCompiler, "./Bytecode.bin");
	if (!bResult)
	{
		CloseCompiler(hCompiler);
		ExitProcess(-1);
	}

	CloseCompiler(hCompiler);
	return 0;
}