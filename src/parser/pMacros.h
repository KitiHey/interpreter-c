#pragma once
#include <string.h>
#include "tools.h"

#define PEEK() (*Lexer)->type
#define PEEK_NEXT() ((*Lexer)+1)->type
#define PEEK_LIT() (*Lexer)->literal
#define CONSUME() ((*Lexer)++)
#define SKIPSEMI() while (PEEK() == SEMICOLON) { CONSUME(); };
#define MALLOC(size) ArenaPush(Arena, size)
#define FREE(Arena) ArenaFree(Arena)
#define STRLEN(character) (strlen(character) + 1)
#define STRDUP(character) Strdup(Arena, character);
