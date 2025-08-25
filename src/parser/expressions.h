#include "parser.h"
#include "ast.h"
#include "tokens.h"

identsexpressions_t* ParseMultipleIdents(token_t** Lexer, arena_t* Arena, tokenType_t delimiter, tokenType_t spacing);

funcexpr_t* ParseFuncExpr(token_t** Lexer, arena_t* Arena);

ifexpr_t* ParseIfExpr(token_t** Lexer, arena_t* Arena);

identexpr_t* ParseIdentExpr(token_t** Lexer, arena_t* Arena);

integerexpr_t* ParseIntExpr(token_t** Lexer, arena_t* Arena);

stringexpr_t* ParseStringExpr(token_t** Lexer, arena_t* Arena);

prefixexpr_t *ParsePrefixExpr(token_t** Lexer, arena_t* Arena);

infixexpr_t *ParseInfixExpr(token_t** Lexer, arena_t* Arena, expressions_t* leftExpr);

expressions_t* ParseExpression(token_t** Lexer, arena_t* Arena, operators_priorities_t Priority);
