#pragma once
#include "tokens.h"
#include "stddef.h"
#include "arena.h"
#include <stdlib.h>

typedef struct LexerStruct {
		token_t* tokens;
		arena_t* arena;
} lexer_t;

lexer_t* Lexer(char* codeFile);
#define FREE_LEXER(lexerStruct) ArenaFree(lexerStruct->arena); \
		lexerStruct->arena = NULL; \
		lexerStruct->tokens = NULL; \
		lexerStruct = NULL;
