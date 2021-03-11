#ifndef MESSAGE_CHANNEL_H
#define MESSAGE_CHANNEL_H

#define MESSAGE_CHANNEL_BAD_RESULT ((DWORD)(-1))

typedef PVOID MESSAGE_CHANNEL;

/**
* � ����� ������ ��������� ����� ������������ �� ������
*/
typedef enum _EMCBufferAllocated
{
	EMCBA_STACK_ALLOCATED,

	EMCBA_HEAP_ALLOCATED
} EMCBufferAllocated, * PEMCBufferAllocated;

/**
* ������� ����� ���������
* @param[out]   hMessageChannel		��������� ������ ���������.
* @return ��������� ������ �������.
*/
BOOL
CreateMessageChannel(
	MESSAGE_CHANNEL* hMessageChannel
);

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
);

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
);

/**
* �������� ������ �������� ��������� � ������
* @param[in]   hMessageChannel		��������� ������ ���������.
* @return ������
*/
DWORD
MCGetCurrentBufferSize(
	MESSAGE_CHANNEL hMessageChannel
);

/**
* ������� ����� ���������
* @param[in]   hMessageChannel		��������� ������ ���������.
*/
VOID
CloseMessageChannel(
	MESSAGE_CHANNEL hMessageChannel
);




#endif