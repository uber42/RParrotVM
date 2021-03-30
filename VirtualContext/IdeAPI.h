#ifndef IDE_API_H
#define IDE_API_H

typedef enum _EIdeApiMessageType
{
	EIAMT_INFO,
	EIAMT_ERROR,
	EIAMT_LEXICAL_ERROR,
	EIAMT_SYNTACTIC_ERROR,
	EIAMT_RUNTIME_ERROR
} EIdeApiMessageType, *PEIdeApiMessageType;


BOOL
InitializeIdeApi();

VOID
DeinitializeIdeApi();

BOOL
IdeApiSendMessage(
	EIdeApiMessageType	eType,
	PCHAR				szMessage
);


#endif