#include "global.h"


static const SStateMahcineCommand machine_operators[] =
{
	{ 0 ,	"new",		0x28999611 },
	{ 1 ,	"set",		0xC6270703 },

	{ 2 ,	"inc",		0xA8E99C47 },
	{ 3 ,	"dec",		0xC25979D3 },
	{ 4 ,	"add",		0x3B391274 },
	{ 5 ,	"sub",		0xDC4E3915 },
	{ 6 ,	"mul",		0xEB84ED81 },
	{ 7 ,	"div",		0xE562AB48 },

	{ 8 ,	"length",	0x83D03615 },
	{ 9 ,	"concat",	0xF5CF8C7D },
	{ 10 ,	"substr",	0x4981C820 },

	{ 11 ,	"branch",	0xB6873945 },
	{ 12 ,	"if",		0x39386E06 },
	{ 13 ,	"ne",		0x5836603C },
	{ 14 ,	"eq",		0x441A6A43 },
	{ 15 ,	"gt",		0x4B208576 },
	{ 16 ,	"lt",		0x5D31EAED },

	{ 17 ,	"bsr",		0x5EE476F0 },
	{ -1 ,	"ret",		0x30F467AC },

	{ 18 ,	"print",	0x16378A88 },

	{ 19 ,	"push",		0x876FFFDD },
	{ 20 ,	"pop",		0x51335FD0 },

	{ -1 ,	"end",		0x6A8E75AA },

	{ 21,	"typeof",	0x9A90A8A0 },
	{ 22,	"erase",	0x3C41DDD5 },
	{ 23,	"chk",		0xF385873F }
};

static SMiddleLayerContainer tables[9] = { 0 };

static const SMiddleStateLayer set_table[] =
{
	{ EMT_VIRTUAL_MEMORY, NATIVE_REG_MASK | ALL_LITERAL_MASK,			{ .nNextState = -1 },		14,		 1,	ENST_NUMBER, EPO_SET	},
	{ EMT_I, EMT_I | EMT_P | EMT_NUMBER_LITERAL | EMT_VIRTUAL_MEMORY,	{ .nNextState = -1 },		14,		 1,	ENST_NUMBER, EPO_SET	},
	{ EMT_N, EMT_N | EMT_P | EMT_NUMBER_LITERAL | EMT_VIRTUAL_MEMORY,	{ .nNextState = -1 },		14,		 1,	ENST_NUMBER, EPO_SET	},
	{ EMT_S, EMT_S | EMT_STRING_LITERAL,								{ .nNextState = -1 },		14,		 1,	ENST_NUMBER, EPO_SET 	},
	{ EMT_P, ALL_MEMORY_MASK | ALL_LITERAL_MASK,						{ .nNextState = -1 },		14,		 1,	ENST_NUMBER, EPO_SET	}
};

static const SMiddleStateLayer add_table[] =
{
	{ EMT_I | EMT_N | EMT_P, ALL_NUMBER_MASK,	{ .sTable = tables + 2	},		15,		1, ENST_TABLE, EPO_ADD_2 }
};

static const SMiddleStateLayer add_table_2[] =
{
	{ ALL_NUMBER_MASK,		ALL_NUMBER_MASK,	{ .nNextState = -1		},		15,		1, ENST_NUMBER, EPO_ADD_3 },
	{ EMT_NUMBER_LITERAL, 0,					{ .nNextState = -1		},		15,		1, ENST_NUMBER, EPO_ADD_2 }
};

static const SMiddleStateLayer sub_table[] =
{
	{ EMT_I | EMT_N | EMT_P, ALL_NUMBER_MASK,	{ .sTable = tables + 4	},		15,		1, ENST_TABLE, EPO_SUB_2 }
};

static const SMiddleStateLayer sub_table_2[] =
{
	{ ALL_NUMBER_MASK,		ALL_NUMBER_MASK,	{ .nNextState = -1		},		15,		1, ENST_NUMBER, EPO_SUB_3 },
	{ EMT_NUMBER_LITERAL, 0,					{ .nNextState = -1		},		15,		1, ENST_NUMBER, EPO_SUB_2 }
};

static const SMiddleStateLayer mul_table[] =
{
	{ EMT_I | EMT_N | EMT_P, ALL_NUMBER_MASK,	{ .sTable = tables + 6	},		15,		1, ENST_TABLE, EPO_MUL_2 }
};

static const SMiddleStateLayer mul_table_2[] =
{
	{ ALL_NUMBER_MASK,		ALL_NUMBER_MASK,	{ .nNextState = -1		},		15,		1, ENST_NUMBER, EPO_MUL_3 },
	{ EMT_NUMBER_LITERAL, 0,					{ .nNextState = -1		},		15,		1, ENST_NUMBER, EPO_MUL_2 }
};

static const SMiddleStateLayer div_table[] =
{
	{ EMT_I | EMT_N | EMT_P, ALL_NUMBER_MASK,	{ .sTable = tables + 8	},		15,		1, ENST_TABLE, EPO_DIV_2 }
};

static const SMiddleStateLayer div_table_2[] =
{
	{ ALL_NUMBER_MASK,		ALL_NUMBER_MASK,	{ .nNextState = -1		},		15,		1, ENST_NUMBER, EPO_DIV_3 },
	{ EMT_NUMBER_LITERAL, 0,					{ .nNextState = -1		},		15,		1, ENST_NUMBER, EPO_DIV_2 }
};

static const SStateMachineTransition transition_1[] =
{
	{ EMT_P,								{ .nNextState = 0			},		0,		0,		ENST_NUMBER,	0				},		// new [REG], [PMC_TYPE]
	{ ALL_MEMORY_MASK | EMT_P,				{ .sTable	  = tables		},		-1,		0,		ENST_TABLE,		0				},		// set [REG, MEM], [REG, MEM]

	{ EMT_I | EMT_N | EMT_P,				{ .nNextState = -1			},		1,		1,		ENST_NUMBER,	EPO_INC			},		// inc [REG]
	{ EMT_I | EMT_N | EMT_P,				{ .nNextState = -1			},		1,		1,		ENST_NUMBER,	EPO_DEC			},		// dec [REG]
	{ EMT_I | EMT_N | EMT_P,				{ .sTable	  = tables + 1	},		2,		0,		ENST_TABLE,		0				},		// add [REG], [REG, NUMBER_LITERAL]!, [REG, NUMBER_LITERAL]
	{ EMT_I | EMT_N | EMT_P,				{ .sTable	  = tables + 3	},		3,		0,		ENST_TABLE,		0				},		// sub [REG], [REG, NUMBER_LITERAL]!, [REG, NUMBER_LITERAL]
	{ EMT_I | EMT_N | EMT_P,				{ .sTable	  = tables + 5	},		4,		0,		ENST_TABLE,		0				},		// mul [REG], [REG, NUMBER_LITERAL]!, [REG, NUMBER_LITERAL]
	{ EMT_I | EMT_N | EMT_P,				{ .sTable	  = tables + 7	},		5,		0,		ENST_TABLE,		0				},		// div [REG], [REG, NUMBER_LITERAL]!, [REG, NUMBER_LITERAL]

	{ EMT_I,								{ .nNextState = 1			},		6,		0,		ENST_NUMBER,	0				},		// length [REG], [REG, STRING_LITERAL]
	{ EMT_S | EMT_P,						{ .nNextState = 2			},		7,		0,		ENST_NUMBER,	0				},		// concat [REG], [REG, STRING_LITERAL]
	{ EMT_S | EMT_P,						{ .nNextState = 3			},		8,		0,		ENST_NUMBER,	0				},		// substr [REG], [REG, STRING_LITERAL], [REG, NUMBER_LITERAL], [REG, NUMBER_LITERAL]

	{ EMT_MARKER,							{ .nNextState = -1			},		9,		1,		ENST_NUMBER,	EPO_BRANCH		},		// branch [MARKER]
	{ EMT_I | EMT_NUMBER_LITERAL,			{ .nNextState = 4			},		10,		0,		ENST_NUMBER,	0				},		// if [REG, NUMBER_LITERAL], [MARKER], [MARKER]
	{ REGISTERS_AND_LITERAL,				{ .nNextState = 5			},		11,		0,		ENST_NUMBER,	0				},		// ne [REG, ANY_LITERAL], [REG, ANY_LITERAL], [MARKER], [MARKER]
	{ REGISTERS_AND_LITERAL,				{ .nNextState = 6			},		11,		0,		ENST_NUMBER,	0				},		// eq [REG, ANY_LITERAL], [REG, ANY_LITERAL], [MARKER], [MARKER]
	{ ALL_NUMBER_MASK,						{ .nNextState = 7			},		11,		0,		ENST_NUMBER,	0				},		// gt [REG, NUMBER_LITERAL], [REG, NUMBER_LITERAL], [MARKER], [MARKER]
	{ ALL_NUMBER_MASK,						{ .nNextState = 8			},		11,		0,		ENST_NUMBER,	0				},		// lt [REG, NUMBER_LITERAL], [REG, NUMBER_LITERAL], [MARKER], [MARKER]

	{ EMT_MARKER,							{ .nNextState = -1			},		9,		1,		ENST_NUMBER,	EPO_BSR			},		// bsr [MARKER]

	{ NATIVE_MASK | EMT_P,					{ .nNextState = -1			},		12,		1,		ENST_NUMBER,	EPO_PRINT		},		// print [REG, STRING_LITERAL]

	{ EMT_NUMBER_LITERAL | EMT_I | EMT_P,	{ .nNextState = 9			},		13,		1,		ENST_NUMBER,	EPO_PUSH_STACK	},		// push
	{ REGISTERS_MASK,						{ .nNextState = 10			},		13,		1,		ENST_NUMBER,	EPO_POP_STACK	},		// pop

	{ EMT_S,								{ .nNextState = 11			},		77,		0,		ENST_NUMBER,	0				},		// typeof	[STR_REG], [PMC_REGISTER]
	{ EMT_P,								{ .nNextState = 12			},		77,		0,		ENST_NUMBER,	0				},		// erase	[PMC_REG], [STRING]
	{ EMT_MARKER,							{ .nNextState = -1			},		77,		1,		ENST_NUMBER,	EPO_CHK			}		// chk		[MARKER]
};

static const SStateMachineTransition transition_2[] =
{
	{ PMC_TYPE_MASK,						{ .nNextState = -1 },		16,		1,		ENST_NUMBER, EPO_NEW		},		// new

	{ EMT_S | EMT_STRING_LITERAL,			{ .nNextState = 8  },		17,		1,		ENST_NUMBER, EPO_LENGTH		},		// length
	{ EMT_S | EMT_STRING_LITERAL,			{ .nNextState = -1 },		18,		1,		ENST_NUMBER, EPO_CONCAT		},		// concat
	{ EMT_S | EMT_STRING_LITERAL,			{ .nNextState = 0  },		19,		0,		ENST_NUMBER, 0				},		// substr

	{ EMT_MARKER,							{ .nNextState = 1  },		20,		1,		ENST_NUMBER, EPO_IF_2		},		// if
	{ REGISTERS_AND_LITERAL,				{ .nNextState = 2  },		20,		0,		ENST_NUMBER, 0				},		// ne
	{ REGISTERS_AND_LITERAL,				{ .nNextState = 3  },		20,		0,		ENST_NUMBER, 0				},		// eq
	{ ALL_NUMBER_MASK,						{ .nNextState = 4  },		20,		0,		ENST_NUMBER, 0				},		// gt
	{ ALL_NUMBER_MASK,						{ .nNextState = 5  },		20,		0,		ENST_NUMBER, 0				},		// lt
			
	{ EMT_S | EMT_STRING_LITERAL,			{ .nNextState = 6  },		77,		0,		ENST_NUMBER, 0				},		// push
	{ EMT_P,								{ .nNextState = 7  },		77,		0,		ENST_NUMBER, 0				},		// pop

	{ EMT_P,								{ .nNextState = -1 },		77,		1,		ENST_NUMBER, EPO_TYPEOF		},		// typeof
	{ EMT_S | EMT_STRING_LITERAL,			{ .nNextState = -1 },		77,		1,		ENST_NUMBER, EPO_PMC_ERASE	}		// erase
};

static const SStateMachineTransition transition_3[] =
{
	{ INTEGERS_MASK,						{ .nNextState = 0  },		21,		0,		ENST_NUMBER, 0				},	// substr

	{ EMT_MARKER,							{ .nNextState = -1 },		22,		1,		ENST_NUMBER, EPO_IF_3		},	// if
	{ EMT_MARKER,							{ .nNextState = 1  },		22,		1,		ENST_NUMBER, EPO_NE_3		},	// ne
	{ EMT_MARKER,							{ .nNextState = 2  },		22,		1,		ENST_NUMBER, EPO_EQ_3		},	// eq
	{ EMT_MARKER,							{ .nNextState = 3  },		22,		1,		ENST_NUMBER, EPO_GT_3		},	// gt
	{ EMT_MARKER,							{ .nNextState = 4  },		22,		1,		ENST_NUMBER, EPO_LT_3		},	// lt

	{ ALL_LITERAL_MASK | REGISTERS_MASK,	{ .nNextState = -1 },		77,		1,		ENST_NUMBER, EPO_PMC_PUSH	},	// push
	{ EMT_S | EMT_STRING_LITERAL,			{ .nNextState = -1 },		77,		1,		ENST_NUMBER, EPO_PMC_POP	},	// pop

	{ EMT_S | EMT_STRING_LITERAL,			{ .nNextState = -1 },		77,		1,		ENST_NUMBER, EPO_LENGTH		}	// length
};

static const SStateMachineTransition transition_4[] =
{
	{ EMT_I | EMT_NUMBER_LITERAL,			{ .nNextState = -1 },		21,		1,		ENST_NUMBER, EPO_SUBSTR		},	// substr
	{ EMT_MARKER,							{ .nNextState = -1 },		22,		1,		ENST_NUMBER, EPO_NE_4		},	// ne
	{ EMT_MARKER,							{ .nNextState = -1 },		22,		1,		ENST_NUMBER, EPO_EQ_4		},	// eq
	{ EMT_MARKER,							{ .nNextState = -1 },		22,		1,		ENST_NUMBER, EPO_GT_4		},	// gt
	{ EMT_MARKER,							{ .nNextState = -1 },		22,		1,		ENST_NUMBER, EPO_LT_4		}	// lt
};

static const PSStateMachineTransition trasitions[] =
{
	transition_1, transition_2, 
	transition_3, transition_4
};

static const char errorMessages[][STRING_MAX_LENGTH] =
{
	"Команда new предназначена только для регистов P",							// 0

	"Инкремент производится только для числовых регистров",						// 1
	"Декремент производится только для числовых регистров",						// 2
	"Сложение выполняется только для числовых значений",						// 3
	"Вычитание выполняется только для числовых значений",						// 4
	"Умножение выполняется только для числовых значений",						// 5
	"Деление выполняется только для числовых значений",							// 6

	"Длина строки должна записываться в регистр",								// 7
	"Конкатенация выполняется только для строк",								// 8
	"Оператор подстроки может принимать только строку для параметра",			// 9

	"Должна быть указана метка",												// 10
	"Для условия принимается целочисленное значение",							// 11
	"Для условия может приниматься только регистр",								// 12

	"В стек можно добавить только значение или регистр",						// 13
	"Из стека можно записать только в регистр",									// 14

	"Ошибка set",																// 15
	"Ошибка арифметической операции",											// 16

	"Оператор длины строки должен принимать строку или строковый регистр",		// 17
	"Оператор конкатенации должен принимать строку или строковый регистр",		// 18

	"Оператор подстроки должен принимать строку или строковый регистр",			// 19
	"Условие должно принимать число или числовой регистр",						// 20

	"Оператор подстроки должен принимать строку или строковый регистр",			// 21
	"Оператор условия должен содержать маркер"									// 22
};


VOID
InititalizeStateMachineTables()
{
	tables[0] = (SMiddleLayerContainer) { .psTable = set_table,		.dwCount = 5 };

	tables[1] = (SMiddleLayerContainer) { .psTable = add_table,		.dwCount = 1 };
	tables[2] = (SMiddleLayerContainer) { .psTable = add_table_2,	.dwCount = 2 };
	tables[3] = (SMiddleLayerContainer) { .psTable = sub_table,		.dwCount = 1 };
	tables[4] = (SMiddleLayerContainer) { .psTable = sub_table_2,	.dwCount = 2 };
	tables[5] = (SMiddleLayerContainer) { .psTable = mul_table,		.dwCount = 1 };
	tables[6] = (SMiddleLayerContainer) { .psTable = mul_table_2,	.dwCount = 2 };
	tables[7] = (SMiddleLayerContainer) { .psTable = div_table,		.dwCount = 1 };
	tables[8] = (SMiddleLayerContainer) { .psTable = div_table_2,	.dwCount = 2 };
}

static 
BOOL
HandleMiddleTransitionTable(
	PSMiddleLayerContainer	psMiddleStateLayer,
	PSLexemeContainer		psLexemeContainer,
	ETokenType				ePrevState,
	DWORD					dwCurrentLexeme
)
{
	if (dwCurrentLexeme >= psLexemeContainer->dwCount)
	{
		return FALSE;
	}

	PSMiddleStateLayer psTable = (PSMiddleStateLayer)psMiddleStateLayer->psTable;
	DWORD dwCurrentToken = psLexemeContainer->eToken[dwCurrentLexeme];
	DWORD dwNeedTokenIdx = (DWORD)-1;
	for (DWORD i = 0; i < psMiddleStateLayer->dwCount; i++)
	{
		if ((ePrevState & psTable[i].ePrevToken) == ePrevState)
		{
			dwNeedTokenIdx = i;
			break;
		}
	}

	if (dwNeedTokenIdx == (DWORD)-1)
	{
		return FALSE;
	}

	psLexemeContainer->eEndPointOperation = psTable[dwNeedTokenIdx].eEndPointOp;
	ETokenType eCurrentToken = psLexemeContainer->eToken[dwCurrentLexeme];
	if ((psTable[dwNeedTokenIdx].eTokenType & eCurrentToken) != eCurrentToken)
	{
		return FALSE;
	}

	if (dwCurrentLexeme == psLexemeContainer->dwCount - 1)
	{
		return psTable[dwNeedTokenIdx].bMayEnd;
	}

	return HandleMiddleTransitionTable(
		psTable[dwNeedTokenIdx].uNextState.sTable,
		psLexemeContainer, eCurrentToken,
		dwCurrentLexeme + 1);
}

BOOL
StateMachineDriveLexemes(
	PSLexemeContainer	psLexemeContainer
)
{
	ETokenType eCurrentToken = psLexemeContainer->eToken[0];
	if (eCurrentToken != EMT_COMMAND)
	{
		return FALSE;
	}

	BOOL bStateFound = FALSE;
	DWORD dwCurrentState = 0;
	DWORD dwIndexFound = 0;
	DWORD dwLength = strlen(psLexemeContainer->szLexemes[0]);
	DWORD dwHash = Fnv1aCompute(psLexemeContainer->szLexemes[0], dwLength, sizeof(CHAR), FNV_1A_SEED);
	for (DWORD i = 0; i < sizeof(machine_operators) / sizeof(SStateMahcineCommand); i++)
	{
		if (dwHash == machine_operators[i].dwHash)
		{
			bStateFound = TRUE;
			dwCurrentState = machine_operators[i].nOpCode;
			dwIndexFound = i;
			break;
		}
	}

	if (!bStateFound)
	{
		return FALSE;
	}

	if (dwCurrentState == (WORD)-1)
	{
		switch (machine_operators[dwIndexFound].dwHash)
		{
		case 0x30F467AC:
			psLexemeContainer->eEndPointOperation = EPO_RET;
			break;
		case 0x6A8E75AA:
			psLexemeContainer->eEndPointOperation = EPO_END;
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}

	DWORD dwCurrentLexeme = 1;
	DWORD dwIterations = psLexemeContainer->dwCount - 1;
	psLexemeContainer->eEndPointOperation = trasitions[0][dwCurrentState].eEndPointOp;
	for (DWORD i = 0; i < dwIterations; i++)
	{
		DWORD dwNeedToken = trasitions[i][dwCurrentState].eTokenType;
		DWORD dwCurrentLexemeToken = psLexemeContainer->eToken[dwCurrentLexeme];
		if ((dwNeedToken & dwCurrentLexemeToken) != dwCurrentLexemeToken)
		{
			return FALSE;
		}

		ENextStateType eNextStateType = trasitions[i][dwCurrentState].eTransitionType;
		if (eNextStateType == ENST_NUMBER)
		{
			DWORD dwNextState = trasitions[i][dwCurrentState].uNextState.nNextState;
			if (dwNextState == (DWORD)-1 && i + 2 != psLexemeContainer->dwCount)
			{
				return FALSE;
			}

			psLexemeContainer->eEndPointOperation = trasitions[i][dwCurrentState].eEndPointOp;
			if (psLexemeContainer->dwCount == i + 2)
			{
				return trasitions[i][dwCurrentState].bMayEnd;
			}

			dwCurrentState = dwNextState;
			dwCurrentLexeme++;
		}
		else
		{
			return HandleMiddleTransitionTable(
				trasitions[i][dwCurrentState].uNextState.sTable,
				psLexemeContainer, dwCurrentLexemeToken, dwCurrentLexeme + 1);
		}
	}

	return TRUE;
}