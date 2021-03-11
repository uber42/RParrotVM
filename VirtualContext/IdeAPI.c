#include "global.h"

#define SHARED_MEMORY_NAME  TEXT("PasmIdeSharedMemory")
#define RECV_EVENT_NAME		TEXT("PasmIdeRecvEvent")
#define SEND_EVENT_NAME		TEXT("PasmIdeSendEvent")

typedef struct _SIdeMessage
{
	DWORD dwType;

	CHAR  szMessage[STRING_MAX_LENGTH];
} SIdeMessage, * PSIdeMessage;

static HANDLE			g_hThread = NULL;

static HANDLE			g_hSharedMemory = NULL;

static MESSAGE_CHANNEL	g_hMessageQueue = NULL;

static PSIdeMessage		g_psMsgBuffer = NULL;

static HANDLE			g_hRecvEvent = NULL;

static HANDLE			g_hSendEvent = NULL;

static BOOL				g_isInited = FALSE;

static
DWORD
WINAPI
ThreadRoutine(
	LPVOID lpArgument
);

BOOL
InitializeIdeApi()
{
	DWORD dwError = 0;
	BOOL bResult = CreateMessageChannel(&g_hMessageQueue);
	if (!bResult)
	{
		return FALSE;
	}

	g_hSharedMemory = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		TRUE,
		SHARED_MEMORY_NAME);
	if (!g_hSharedMemory)
	{
		CloseMessageChannel(g_hMessageQueue);
		return FALSE;
	}

	g_psMsgBuffer = (PSIdeMessage)MapViewOfFile(
		g_hSharedMemory,  
		FILE_MAP_WRITE, 
		0, 0, sizeof(SIdeMessage));
	if (!g_psMsgBuffer)
	{
		CloseHandle(g_hSharedMemory);
		CloseMessageChannel(g_hMessageQueue);
		return FALSE;
	}

	g_hRecvEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, RECV_EVENT_NAME);
	if (!g_hRecvEvent)
	{
		CloseHandle(g_hSharedMemory);
		CloseMessageChannel(g_hMessageQueue);
		return FALSE;
	}

	g_hSendEvent = OpenMutex(MUTEX_ALL_ACCESS, TRUE, SEND_EVENT_NAME);
	if (!g_hSendEvent)
	{
		CloseHandle(g_hRecvEvent);
		CloseHandle(g_hSharedMemory);
		CloseMessageChannel(g_hMessageQueue);
		return FALSE;
	}

	g_hThread = CreateThread(
		NULL, 0, ThreadRoutine,
		NULL, 0, NULL);
	if (!g_hThread)
	{
		CloseHandle(g_hSendEvent);
		CloseHandle(g_hRecvEvent);
		CloseHandle(g_hSharedMemory);
		CloseMessageChannel(g_hMessageQueue);
		return FALSE;
	}

	g_isInited = TRUE;
	return TRUE;
}

static
DWORD
WINAPI
ThreadRoutine(
	LPVOID lpArgument
)
{
	while (TRUE)
	{
		DWORD dwSize = 0;
		SIdeMessage sIdeMessage = { 0 };
		ReceiveMessageChannel(
			g_hMessageQueue,
			&sIdeMessage,
			&dwSize);

		memcpy(g_psMsgBuffer, &sIdeMessage, sizeof(SIdeMessage));

		BOOL bResult = SetEvent(g_hRecvEvent);
		if (!bResult)
		{
			return FALSE;
		}

		WaitForSingleObject(g_hSendEvent, INFINITE);
	}
}

VOID
DeinitializeIdeApi()
{
	CloseMessageChannel(g_hMessageQueue);

	CloseHandle(g_hSendEvent);
	CloseHandle(g_hRecvEvent);
	CloseHandle(g_hSharedMemory);

	WaitForSingleObject(g_hThread, INFINITE);
	CloseHandle(g_hThread);
}

BOOL
IdeApiSendMessage(
	EIdeApiMessageType	eType,
	PCHAR				szMessage
)
{
	if (!g_isInited)
	{
		return TRUE;
	}

	SIdeMessage sIdeMessage;
	sIdeMessage.dwType = (DWORD)eType;
	strcpy(sIdeMessage.szMessage, szMessage);

	DWORD dwError = 0;
	return SendMessageChannel(
		g_hMessageQueue, 0,
		&sIdeMessage,
		sizeof(SIdeMessage),
		EMCBA_STACK_ALLOCATED);
}