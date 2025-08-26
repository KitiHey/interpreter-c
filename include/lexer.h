#pragma once
#include "tokens.h"
#include "stddef.h"
#include "arena.h"

typedef struct LexerStruct {
		token_t* tokens;
		arena_t* arena;
} lexer_t;

lexer_t* Lexer(char* codeFile);
