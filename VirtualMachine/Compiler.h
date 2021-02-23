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

VOID
CloseCompiler(
	COMPILER	hCompiler
);

#endif