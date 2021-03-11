#ifndef GLOBAL_H
#define GLOBAL_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "../VirtualContext/List.h"
#include "../VirtualContext/Fnv1a.h"

#define STRING_MAX_LENGTH	256

#include "../VirtualContext/Defenitions.h"
#include "../VirtualContext/HashTable.h"
#include "../VirtualContext/PMC.h"
#include "../VirtualContext/VirtualContext.h"
#include "../VirtualContext/IdeAPI.h"

#include "Token.h"
#include "Lexer.h"
#include "StateMachine.h"
#include "Compiler.h"

#include "CompilerService.h"

#include "LexerTest.h"
#include "StateMachineTest.h"

#endif