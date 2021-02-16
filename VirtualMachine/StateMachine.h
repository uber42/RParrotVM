#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H


typedef struct _SStateMachineCommand
{
	/** Код операции */
	WORD	nOpCode;

	/** Имя */
	CHAR	szName[STRING_MAX_LENGTH];

	/** Хэш (FNV1-A) */
	DWORD	dwHash;
} SStateMahcineCommand, *PSStateMachineCommand;

typedef enum _ENextStateType
{
	/** Номер перехода */
	ENST_NUMBER,

	/** Таблица переходов */
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
		WORD nNextState;

		/** Переход на список состояний */
		struct _SMiddleStateLayer* sTable;
	} uNextState;

	/** Номер ошибки (Если закончилось без ошибки то 0) */
	WORD					nErrorNumber;

	/** Конец или нет */
	BOOL					bMayEnd;

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
		WORD				nNextState;

		/** Переход на список состояний */
		SMiddleStateLayer*	sTable;
	} uNextState;

	/** Номер ошибки (Если закончилось без ошибки то 0) */
	WORD					nErrorNumber;

	/** Конец или нет */
	BOOL					bMayEnd;

	/** Тип перехода */
	ENextStateType			eTransitionType;
} SStateMachineTransition, *PSStateMachineTransition;


#endif