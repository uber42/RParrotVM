#ifndef COMPILER_SERIVCE_H
#define COMPILER_SERVICE_H


BOOL
InitializeCompilerService(
	INT			nArgc,
	PCHAR*		pszArgv
);

BOOL
CompilerServiceStart();

BOOL
DeinitializeCompilerService();









#endif