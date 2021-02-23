#include "global.h"




int main()
{
	COMPILER hCompiler = NULL;
	BOOL bResult = CreateCompiler(
		&hCompiler, "C:\\Users\\uber42\\source\\repos\\Parrot\\VirtualMachine\\Examples\\loop.pasm");
	if (!bResult)
	{
		return FALSE;
	}

	bResult = CompileProgram(hCompiler);
	assert(bResult);

	CloseCompiler(hCompiler);
	return TRUE;
}