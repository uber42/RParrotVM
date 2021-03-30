#ifndef VM_SERVICE_H
#define VM_SERVICE_H


BOOL
InitializeVmService(
	INT			nArgc,
	PCHAR*		pszArgv
);

BOOL
VmServiceStart();

VOID
DeinitializeVmService();


#endif