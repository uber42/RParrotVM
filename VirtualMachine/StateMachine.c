#include "global.h"


static const SStateMahcineCommand commands[][16] =
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

	{ -1 ,	"end",		0x6A8E75AA }
};

static const SMiddleStateLayer set_table[] =
{
	{ EMT_VIRTUAL_MEMORY, REGISTERS_MASK | ALL_LITERAL_MASK,	{ .nNextState = -1 },		14,		 1,	ENST_NUMBER },
	{ EMT_I, EMT_I | EMT_P | EMT_NUMBER_LITERAL,				{ .nNextState = -1 },		14,		 1,	ENST_NUMBER	},
	{ EMT_N, EMT_N | EMT_P | EMT_NUMBER_LITERAL,				{ .nNextState = -1 },		14,		 1,	ENST_NUMBER	},
	{ EMT_S, EMT_N | EMT_P | EMT_NUMBER_LITERAL,				{ .nNextState = -1 },		14,		 1,	ENST_NUMBER	},
	{ EMT_P, ALL_MEMORY_MASK,									{ .nNextState = -1 },		14,		 1,	ENST_NUMBER }
};

static const SMiddleStateLayer math_table[] =
{
	{ EMT_I, EMT_I | EMT_NUMBER_LITERAL, { .sTable = &math_table },		15,		1, ENST_TABLE },
	{ EMT_N, EMT_N | EMT_NUMBER_LITERAL, { .sTable = &math_table },		15,		1, ENST_TABLE },
	{ EMT_P, EMT_P | EMT_NUMBER_LITERAL, { .sTable = &math_table },		15,		1, ENST_TABLE },
	{ EMT_NUMBER_LITERAL, -1,			 { .nNextState = -1		 },		15,		1, ENST_NUMBER }
};

static const SStateMachineTransition transition_1[] =
{
	{ EMT_P,						{ .nNextState = 0			},		0,		0,		ENST_NUMBER },		// new [REG], [PMC_TYPE]
	{ ALL_MEMORY_MASK,				{ .sTable	  = &set_table	},		-1,		0,		ENST_TABLE	},		// set [REG, MEM], [REG, MEM]

	{ EMT_I | EMT_N | EMT_P,		{ .nNextState = -1			},		1,		1,		ENST_NUMBER },		// inc [REG]
	{ EMT_I | EMT_N | EMT_P,		{ .nNextState = -1			},		1,		1,		ENST_NUMBER },		// dec [REG]
	{ EMT_I | EMT_N | EMT_P,		{ .sTable	  = &math_table	},		2,		0,		ENST_TABLE  },		// add [REG], [REG, NUMBER_LITERAL]!, [REG, NUMBER_LITERAL]
	{ EMT_I | EMT_N | EMT_P,		{ .sTable	  = &math_table	},		3,		0,		ENST_TABLE  },		// sub [REG], [REG, NUMBER_LITERAL]!, [REG, NUMBER_LITERAL]
	{ EMT_I | EMT_N | EMT_P,		{ .sTable	  = &math_table	},		4,		0,		ENST_TABLE	},		// mul [REG], [REG, NUMBER_LITERAL]!, [REG, NUMBER_LITERAL]
	{ EMT_I | EMT_N | EMT_P,		{ .sTable	  = &math_table	},		5,		0,		ENST_TABLE	},		// div [REG], [REG, NUMBER_LITERAL]!, [REG, NUMBER_LITERAL]

	{ EMT_I | EMT_P,				{ .nNextState = 1			},		6,		0,		ENST_NUMBER	},		// length [REG], [REG, STRING_LITERAL]
	{ EMT_S | EMT_P,				{ .nNextState = 2			},		7,		0,		ENST_NUMBER	},		// concat [REG], [REG, STRING_LITERAL]
	{ EMT_S | EMT_P,				{ .nNextState = 3			},		8,		0,		ENST_NUMBER	},		// substr [REG], [REG, STRING_LITERAL], [REG, NUMBER_LITERAL], [REG, NUMBER_LITERAL]

	{ EMT_MARKER,					{ .nNextState = -1			},		9,		1,		ENST_NUMBER	},		// branch [MARKER]
	{ EMT_I | EMT_NUMBER_LITERAL,	{ .nNextState = 4			},		10,		0,		ENST_NUMBER	},		// if [REG, NUMBER_LITERAL], [MARKER], [MARKER]
	{ REGISTERS_AND_LITERAL,		{ .nNextState = 5			},		11,		0,		ENST_NUMBER	},		// ne [REG, ANY_LITERAL], [REG, ANY_LITERAL], [MARKER], [MARKER]
	{ REGISTERS_AND_LITERAL,		{ .nNextState = 6			},		11,		0,		ENST_NUMBER	},		// eq [REG, ANY_LITERAL], [REG, ANY_LITERAL], [MARKER], [MARKER]
	{ REGISTERS_AND_LITERAL,		{ .nNextState = 7			},		11,		0,		ENST_NUMBER	},		// gt [REG, ANY_LITERAL], [REG, ANY_LITERAL], [MARKER], [MARKER]
	{ REGISTERS_AND_LITERAL,		{ .nNextState = 8			},		11,		0,		ENST_NUMBER	},		// lt [REG, ANY_LITERAL], [REG, ANY_LITERAL], [MARKER], [MARKER]

	{ EMT_MARKER,					{ .nNextState = -1			},		9,		1,		ENST_NUMBER	},		// bsr [MARKER]

	{ REGISTERS_AND_LITERAL,		{ .nNextState = -1			},		12,		1,		ENST_NUMBER	},		// print [REG, ANY_LITERAL]

	{ REGISTERS_AND_LITERAL,		{ .nNextState = -1			},		13,		1,		ENST_NUMBER	},		// push [REG, ANY_LITERAL]
	{ REGISTERS_MASK,				{ .nNextState = -1			},		13,		1,		ENST_NUMBER	},		// pop  [REG]
};

static const SStateMachineTransition transition_2[] =
{
	{ EMT_PMC_TYPE,					{ .nNextState = -1 },		16,		1,		ENST_NUMBER },				// new

	{ EMT_S | EMT_STRING_LITERAL,	{ .nNextState = -1 },		17,		1,		ENST_NUMBER },				// length
	{ EMT_S | EMT_STRING_LITERAL,	{ .nNextState = -1 },		18,		1,		ENST_NUMBER },				// concat
	{ EMT_S | EMT_STRING_LITERAL,	{ .nNextState = 0  },		19,		0,		ENST_NUMBER },				// substr

	{ REGISTERS_AND_LITERAL,		{ .nNextState = 1  },		20,		0,		ENST_NUMBER	},				// if
	{ REGISTERS_AND_LITERAL,		{ .nNextState = 1  },		20,		0,		ENST_NUMBER	},				// ne
	{ REGISTERS_AND_LITERAL,		{ .nNextState = 1  },		20,		0,		ENST_NUMBER	},				// eq
	{ REGISTERS_AND_LITERAL,		{ .nNextState = 1  },		20,		0,		ENST_NUMBER	},				// gt
	{ REGISTERS_AND_LITERAL,		{ .nNextState = 1  },		20,		0,		ENST_NUMBER	}				// lt
};

static const SStateMachineTransition transition_3[] =
{
	{ EMT_I | EMT_NUMBER_LITERAL,	{ .nNextState = 0 },		21,		0,		ENST_NUMBER },				// substr
	{ EMT_MARKER,					{ .nNextState = 1 },		22,		1,		ENST_NUMBER }				// if, ne, eq, gt, lt
};

static const SStateMachineTransition transition_4[] =
{
	{ EMT_I | EMT_NUMBER_LITERAL,	{ .nNextState = -1 },		21,		1,		ENST_NUMBER },				// substr
	{ EMT_MARKER,					{ .nNextState = -1 },		22,		1,		ENST_NUMBER }				// if, ne, eq, gt, lt
};

int getNextState()
{
	return transition_1[0].uNextState.nNextState;
}

static const char errorMessages[][STRING_MAX_LENGTH] =
{
	" оманда new предназначена только дл€ регистов P",							// 0

	"»нкремент производитс€ только дл€ числовых регистров",						// 1
	"ƒекремент производитс€ только дл€ числовых регистров",						// 2
	"—ложение выполн€етс€ только дл€ числовых значений",						// 3
	"¬ычитание выполн€етс€ только дл€ числовых значений",						// 4
	"”множение выполн€етс€ только дл€ числовых значений",						// 5
	"ƒеление выполн€етс€ только дл€ числовых значений",							// 6

	"ƒлина строки должна записыватьс€ в регистр",								// 7
	" онкатенаци€ выполн€етс€ только дл€ строк",								// 8
	"ќператор подстроки может принимать только строку дл€ параметра",			// 9

	"ƒолжна быть указана метка",												// 10
	"ƒл€ услови€ принимаетс€ целочисленное значение",							// 11
	"ƒл€ услови€ может приниматьс€ только регистр",								// 12

	"¬ стек можно добавить только значение или регистр",						// 13
	"»з стека можно записать только в регистр",									// 14

	"ќшибка set",																// 15
	"ќшибка арифметической операции",											// 16

	"ќператор длины строки должен принимать строку или строковый регистр",		// 17
	"ќператор конкатенации должен принимать строку или строковый регистр",		// 18

	"ќператор подстроки должен принимать строку или строковый регистр",			// 19
	"”словие должно принимать число или числовой регистр",						// 20

	"ќператор подстроки должен принимать строку или строковый регистр",			// 21
	"ќператор услови€ должен содержать маркер"									// 22
};