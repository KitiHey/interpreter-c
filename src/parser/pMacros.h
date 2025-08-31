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

// Errors
#define ERROR_EQ(str, ...) ERROR(str, ##__VA_ARGS__);
#define ERROR_NEQ(str, ...) ERROR(str, ##__VA_ARGS__);
#define ERROR_NULL(str, ...) ERROR(str, ##__VA_ARGS__);
#define ERROR_MALLOC(str, ...) ERROR(str, ##__VA_ARGS__);

#define ERROR_IFNEQ(ifneq, expected, str, ...) if (ifneq != expected) { ERROR_NEQ(str, ##__VA_ARGS__); return NULL; }
#define ERROR_IFEQ(ifeq, expected, str, ...) if (ifeq == expected) { ERROR_EQ(str, ##__VA_ARGS__); return NULL; }
#define ERROR_IFNULL(isnull, str, ...) if (isnull == NULL) { ERROR_NULL(str, ##__VA_ARGS__); return NULL; }
#define ERROR_IFMALLOC(isnull, str, ...) if (isnull == NULL) { ERROR_MALLOC(str, ##__VA_ARGS__); return NULL; }

#define ERROR(str, ...) printf("PARSER ERROR: " str "\n", ##__VA_ARGS__)
