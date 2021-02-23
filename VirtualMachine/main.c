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
	if (!bResult)
	{
		return FALSE;
	}

	CloseCompiler(hCompiler);
	return TRUE;
	//system("PAUSE");
}