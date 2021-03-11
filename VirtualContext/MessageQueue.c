#include "global.h"

/**
* ��������� ������ ���������
*/
typedef struct _SMessageChannel
{
	/** ������� ������ */
	volatile BOOL		bWork;

	/** ������� �������� ��������� */
	HANDLE				hSendEvent;

	/** ������� �������� ��������� */
	HANDLE				hReceiveEvent;

	/** ������� ��������� �������� ��������� */
	HANDLE				hNextSend;

	/** ��������� �������� ������ �������� ��������� */
	struct
	{
		/** ������� �� �������� */
		BOOL				bSuccess;

		/** �� �������� ��������� */
		DWORD				dwMessageId;

		/** ����� ��� �������� ������� ��������� */
		EMCBufferAllocated	eBufferAllocatedIn;

		/** ������ �������� ��������� */
		PVOID				pCurrentBuffer;

		/** ������ �������� ��������� */
		DWORD				dwCurrentBufferSize;
	} sCurrentData;
} SMessageChannel, * PSMessageChannel;

/**
* ������� ����� ���������
* @param[out]   hMessageChannel		��������� ������ ���������.
* @return ��������� ������ �������.
*/
BOOL
CreateMessageChannel(
	MESSAGE_CHANNEL* hMessageChannel
)
{
	PSMessageChannel psMessageChannel = malloc(sizeof(SMessageChannel));
	if (!psMessageChannel)
	{
		return FALSE;
	}

	psMessageChannel->hReceiveEvent = CreateEvent(
		NULL, FALSE,
		FALSE, NULL);
	if (psMessageChannel->hReceiveEvent == NULL)
	{
		free(psMessageChannel);

		return FALSE;
	}

	psMessageChannel->hSendEvent = CreateEvent(
		NULL, FALSE,
		FALSE, NULL);
	if (psMessageChannel->hSendEvent == NULL)
	{
		CloseHandle(psMessageChannel->hReceiveEvent);
		free(psMessageChannel);

		return FALSE;
	}

	psMessageChannel->hNextSend = CreateEvent(
		NULL, FALSE,
		FALSE, NULL);
	if (psMessageChannel->hNextSend == NULL)
	{
		CloseHandle(psMessageChannel->hSendEvent);
		CloseHandle(psMessageChannel->hReceiveEvent);
		free(psMessageChannel);

		return FALSE;
	}

	memset(&psMessageChannel->sCurrentData,
		0, sizeof(psMessageChannel->sCurrentData));

	*hMessageChannel = psMessageChannel;
	psMessageChannel->bWork = TRUE;

	return TRUE;
}


/**
* ��������� ��������� � �����
* @param[in]   hMessageChannel		��������� ������ ���������.
* @param[in]   dwMessageId			�� ���������.
* @param[in]   pBuffer				�����.
* @param[in]   dwBufferSize			������ ������
* @param[in]   eBufferAllocatedIn	��� ������� ����� (���� ��� ����).
* @return ��������� ������ �������.
*/
BOOL
SendMessageChannel(
	MESSAGE_CHANNEL		hMessageChannel,
	DWORD				dwMessageId,
	PVOID				pBuffer,
	DWORD				dwBufferSize,
	EMCBufferAllocated	eBufferAllocatedIn
)
{
	PSMessageChannel psMessageChannel = (PSMessageChannel)hMessageChannel;
	DWORD dwWaitRes = WaitForSingleObject(psMessageChannel->hReceiveEvent, INFINITE);
	if (!psMessageChannel->bWork || dwWaitRes != WAIT_OBJECT_0)
	{
		return FALSE;
	}

	psMessageChannel->sCurrentData.dwCurrentBufferSize = dwBufferSize;
	psMessageChannel->sCurrentData.dwMessageId = dwMessageId;
	psMessageChannel->sCurrentData.eBufferAllocatedIn = eBufferAllocatedIn;
	psMessageChannel->sCurrentData.pCurrentBuffer = pBuffer;
	psMessageChannel->sCurrentData.bSuccess = FALSE;

	BOOL bResult = SetEvent(psMessageChannel->hSendEvent);
	if (!bResult)
	{
		return FALSE;
	}

	dwWaitRes = WaitForSingleObject(psMessageChannel->hNextSend, INFINITE);
	if (!psMessageChannel->bWork || dwWaitRes != WAIT_OBJECT_0)
	{
		return FALSE;
	}

	return psMessageChannel->sCurrentData.bSuccess;
}


/**
* ������� ��������� �� ������
* @param[in]   hMessageChannel		��������� ������ ���������.
* @param[in]   pBuffer				�����.
* @param[in]   dwBufferSize			������ ������.
* @return �� ���������
*/
DWORD
ReceiveMessageChannel(
	MESSAGE_CHANNEL hMessageChannel,
	PVOID			pBuffer,
	PDWORD			pdwBufferSize
)
{
	PSMessageChannel psMessageChannel = (PSMessageChannel)hMessageChannel;

	BOOL bResult = SetEvent(psMessageChannel->hReceiveEvent);
	if (!bResult)
	{
		SetEvent(psMessageChannel->hNextSend);
		return MESSAGE_CHANNEL_BAD_RESULT;
	}

	DWORD dwWaitRes = WaitForSingleObject(psMessageChannel->hSendEvent, INFINITE);
	if (!psMessageChannel->bWork || dwWaitRes != WAIT_OBJECT_0)
	{
		SetEvent(psMessageChannel->hNextSend);
		return MESSAGE_CHANNEL_BAD_RESULT;
	}

	switch (psMessageChannel->sCurrentData.eBufferAllocatedIn)
	{
	case EMCBA_HEAP_ALLOCATED:
		pBuffer = psMessageChannel->sCurrentData.pCurrentBuffer;
		break;
	case EMCBA_STACK_ALLOCATED:
		memcpy(pBuffer, psMessageChannel->sCurrentData.pCurrentBuffer,
			psMessageChannel->sCurrentData.dwCurrentBufferSize);
		break;
	default:
		SetEvent(psMessageChannel->hNextSend);
		return MESSAGE_CHANNEL_BAD_RESULT;
	}

	*pdwBufferSize = psMessageChannel->sCurrentData.dwCurrentBufferSize;
	DWORD dwMessageId = psMessageChannel->sCurrentData.dwMessageId;

	psMessageChannel->sCurrentData.dwCurrentBufferSize = MESSAGE_CHANNEL_BAD_RESULT;
	psMessageChannel->sCurrentData.bSuccess = TRUE;

	bResult = SetEvent(psMessageChannel->hNextSend);
	if (!bResult)
	{
		return MESSAGE_CHANNEL_BAD_RESULT;
	}

	return dwMessageId;
}


/**
* �������� ������ �������� ��������� � ������
* @param[in]   hMessageChannel		��������� ������ ���������.
* @return ������
*/
DWORD
MCGetCurrentBufferSize(
	MESSAGE_CHANNEL hMessageChannel
)
{
	PSMessageChannel psMessageChannel = (PSMessageChannel)hMessageChannel;
	return psMessageChannel->sCurrentData.dwCurrentBufferSize;
}


/**
* ������� ����� ���������
* @param[in]   hMessageChannel		��������� ������ ���������.
*/
VOID
CloseMessageChannel(
	MESSAGE_CHANNEL hMessageChannel
)
{
	PSMessageChannel psMessageChannel = (PSMessageChannel)hMessageChannel;

	psMessageChannel->bWork = FALSE;

	SetEvent(psMessageChannel->hReceiveEvent);
	SetEvent(psMessageChannel->hSendEvent);
	SetEvent(psMessageChannel->hNextSend);

	CloseHandle(psMessageChannel->hReceiveEvent);
	CloseHandle(psMessageChannel->hSendEvent);
	CloseHandle(psMessageChannel->hNextSend);

	free(psMessageChannel);
}