#pragma once
#include "tokens.h"
#include "ast.h"
#include "arena.h"

program_t Parser(token_t* Lexer);
expressions_t* ParseExpression(token_t** Lexer, arena_t* Arena, operators_priorities_t Priority);
stmts_t *ParseStatements(token_t** Lexer, arena_t* Arena, tokenType_t delimiter);
identexpr_t* ParseIdentExpr(token_t** Lexer, arena_t* Arena);
