#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H


typedef struct _SStateMachineCommand
{
	/** ����� �������� */
	u16 nOpCode;

	/** ��������� �������� */
	i8	szName[STRING_MAX_LENGTH];

	/** ��� (FNV1-A) */
	u32	dwHash;
} SStateMahcineCommand, *PSStateMachineCommand;

typedef struct _SStateMachineTransition
{
	/** ����� */
	//EToken		eToken;

	/** ��� ������ */
	EMemoryType	eMemoryType;

	/** ����� ���������� ��������� */
	u16			nNextState;

	/** ����� ������ (���� ������� � ������ ��������� ��� ����������) */
	u16			nErrorNumber;

	/** ����� ���� �������� ���������� */
	u8			bMayEnd;
} SStateMachineTransition, *PSStateMachineTransition;

#endif