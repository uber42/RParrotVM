#ifndef COMPILER_H
#define COMPILER_H

typedef PVOID COMPILER, *PCOMPILER;

BOOL
CreateCompiler(
	PCOMPILER	hCompilerContext,
	CHAR		szFileName[MAX_PATH]
);

BOOL
CompileProgram(
	COMPILER	hCompilerContext
);

BOOL
SaveBytecodeToFile(
	COMPILER	hCompiler,
	CHAR		szFileName[MAX_PATH]
);

VOID
CloseCompiler(
	COMPILER	hCompiler
);

#endif