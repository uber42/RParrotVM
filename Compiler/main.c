#include "global.h"

static char* test_argv[] = { "", "C:\\Users\\uber42\\source\\repos\\Parrot\\Compiler\\Examples\\hashtable.pasm", "-IDE" };


int main(int argc, char *argv[])
{
	BOOL bResult = InitializeCompilerService(3, test_argv);
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