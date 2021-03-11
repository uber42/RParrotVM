#include "global.h"


int main(int argc, char *argv[])
{
	BOOL bResult = InitializeCompilerService(argc, argv);
	if (!bResult)
	{
		ExitProcess(-1);
	}

	bResult = CompilerServiceStart();
	if (!bResult)
	{
		DeinitializeCompilerService();
		ExitProcess(-1);
	}

	DeinitializeCompilerService();
	ExitProcess(0);
}