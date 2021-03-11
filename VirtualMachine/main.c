#include "global.h"
#include <conio.h>


int main(int argc, char* argv[])
{
	BOOL bResult = InitializeVmService(argc, argv);
	if (!bResult)
	{
		ExitProcess(-1);
	}

	bResult = VmServiceStart();
	if (!bResult)
	{
		DeinitializeVmService();
		ExitProcess(-1);
	}

	DeinitializeVmService();

	_getch();
	ExitProcess(0);
}