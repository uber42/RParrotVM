#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H


typedef struct _SStateMachineCommand
{
	/** Код операции */
	u16 nOpCode;

	/** Имя */
	i8	szName[STRING_MAX_LENGTH];

	/** Хэш (FNV1-A) */
	u32	dwHash;
} SStateMahcineCommand, *PSStateMachineCommand;

typedef enum _ENextStateType
{
	ENST_NUMBER,
	ENST_TABLE
} ENextStateType, *PENextStateType;

typedef struct _SMiddleStateLayer
{
	/** Предыдущий токен */
	ETokenType				ePrevToken;

	/** Тип */
	ETokenType				eTokenType;

	/** Следующее состояние */
	union
	{
		/** Переход на следующую таблицу */
		u16 nNextState;

		/** Переход на список состояний */
		struct _SMiddleStateLayer* sTable;
	} uNextState;

	/** Номер ошибки (Если закончилось без ошибки то 0) */
	u16						nErrorNumber;

	/** Конец или нет */
	u8						bMayEnd;

	/** Тип перехода */
	ENextStateType			eTransitionType;
} SMiddleStateLayer, *PSMiddleStateLayer;

typedef struct _SStateMachineTransition
{
	/** Тип */
	ETokenType				eTokenType;

	/** Следующее состояние */
	union
	{
		/** Переход на следующую таблицу */
		u16					nNextState;

		/** Переход на список состояний */
		SMiddleStateLayer*	sTable;
	} uNextState;

	/** Номер ошибки (Если закончилось без ошибки то 0) */
	u16						nErrorNumber;

	/** Конец или нет */
	u8						bMayEnd;

	/** Тип перехода */
	ENextStateType			eTransitionType;
} SStateMachineTransition, *PSStateMachineTransition;

int getNextState();

#endif