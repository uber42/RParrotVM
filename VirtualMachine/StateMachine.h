#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H


typedef struct _SStateMachineCommand
{
	/** Код операции */
	WORD	nOpCode;

	/** Имя */
	CHAR	szName[16];

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
		WORD				nNextState;

		/** Переход на список состояний */
		VOID*				sTable;
	} uNextState;

	/** Номер ошибки (Если закончилось без ошибки то 0) */
	WORD					nErrorNumber;

	/** Конец или нет */
	BOOL					bMayEnd;

	/** Тип перехода */
	ENextStateType			eTransitionType;

	/** Процессорная операция */
	EProcessorOperation		eEndPointOp;
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
		VOID*				sTable;
	} uNextState;

	/** Номер ошибки (Если закончилось без ошибки то 0) */
	WORD					nErrorNumber;

	/** Конец или нет */
	BOOL					bMayEnd;

	/** Тип перехода */
	ENextStateType			eTransitionType;

	/** Процессорная операция */
	EProcessorOperation		eEndPointOp;
} SStateMachineTransition, *PSStateMachineTransition;

typedef struct _SMiddleLayerContainer
{
	/** Таблица */
	VOID*	psTable;

	/** Количество */
	DWORD	dwCount;
} SMiddleLayerContainer, *PSMiddleLayerContainer;

VOID
InititalizeStateMachineTables();

BOOL
StateMachineDriveLexemes(
	PSLexemeContainer	psLexemeContainer
);

#endif