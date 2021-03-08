#include "global.h"




int main()
{
	COMPILER hCompiler = NULL;
	BOOL bResult = CreateCompiler(
		&hCompiler, "C:\\Users\\admin\\ReducedParrotVirtualMachine\\Compiler\\Examples\\pmc_native.pasm");
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