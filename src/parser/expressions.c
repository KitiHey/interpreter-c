#include "ast.h"
#include "tokens.h"
#include "parser.h"
#include "arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pMacros.h"
#include "expressions.h"

conditionexpressions_t* ParseMultipleExpr(token_t** Lexer, arena_t* Arena, tokenType_t delimiter) {
		expressions_t** Buffer = calloc(10, sizeof(expressions_t*));
		ERROR_IFMALLOC(Buffer);
#ifdef ALLOW_TESTS
		char* TestString = calloc(10,  sizeof(char));
		ERROR_IFMALLOC(TestString);
		int capString = 10;
		int idxString = 0;
#endif
		int cap = 10;
		int idx = 0;
		for (idx = 0; PEEK() != delimiter && PEEK() != TERMINATE; idx++) {
				if (idx >= cap) {
						cap *= 2;
						Buffer = realloc(Buffer, cap*sizeof(expressions_t*));
						ERROR_IFMALLOC(Buffer);
				}
				Buffer[idx] = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
				ERROR_IFFROM(Buffer[idx]);
#ifdef ALLOW_TESTS
				idxString += strlen(Buffer[idx]->testString)+1;
				if (idxString >= capString) {
						while (idxString >= capString) {
								capString *= 2;
						}
						TestString = realloc(TestString, capString * sizeof(char));
						ERROR_IFMALLOC(TestString);
				}
				strcat(TestString, Buffer[idx]->testString);
#endif
		}
		ERROR_IFEQ(PEEK(), TERMINATE);
		conditionexpressions_t* Expr = MALLOC(sizeof(conditionexpressions_t));
		ERROR_IFMALLOC(Expr);
		Expr->Expressions = MALLOC((idx+1)*sizeof(expressions_t*));
		ERROR_IFMALLOC(Expr->Expressions);
#ifdef ALLOW_TESTS
		Expr->testString = MALLOC((idxString+1) * sizeof(char));
		ERROR_IFMALLOC(Expr->testString);
#endif

		memcpy(Expr->Expressions, Buffer, idx+1);
		free(Buffer);
		Buffer=NULL;

#ifdef ALLOW_TESTS
		memcpy(Expr->testString, TestString, idxString+1);
		free(TestString);
		TestString=NULL;
#endif

		return Expr;
}

identsexpressions_t* ParseMultipleIdents(token_t** Lexer, arena_t* Arena, tokenType_t delimiter, tokenType_t spacing) {
		identexpr_t** Buffer = calloc(10, sizeof(identexpr_t*));
		ERROR_IFMALLOC(Buffer);
#ifdef ALLOW_TESTS
		char* TestString = calloc(10,  sizeof(char));
		ERROR_IFMALLOC(TestString);
		int capString = 10;
		int idxString = 0;
#endif
		int cap = 10;
		int idx = 0;
		for (idx = 0; PEEK() != delimiter && PEEK() != TERMINATE; idx++) {
				if (idx >= cap) {
						cap *= 2;
						Buffer = realloc(Buffer, cap*sizeof(identexpr_t*));
						ERROR_IFMALLOC(Buffer);
				}
				if (PEEK() != IDENT) {
						free(Buffer);
#ifdef ALLOW_TESTS
						free(TestString);
#endif
						ERROR_NEQ("Ident");
						return NULL;
				}
				Buffer[idx] = ParseIdentExpr(Lexer, Arena);
				if ( (PEEK() != spacing && PEEK() != delimiter) || Buffer[idx] == NULL) {
						free(Buffer);
#ifdef ALLOW_TESTS
						free(TestString);
#endif
						ERROR_IFFROM(Buffer[idx]);
						ERROR_NEQ("Spacing or Delimiter");
						return NULL;
				}
				if (PEEK() == spacing) {
						CONSUME();
				}
#ifdef ALLOW_TESTS
				char spaces[3];
				if (idxString>0) {
						strcpy(spaces, ", ");
				} else {
						strcpy(spaces, "");
				}
				idxString += STRLEN(Buffer[idx]->testString)+strlen(spaces);
				if (idxString >= capString) {
						while (idxString >= capString) {
								capString *= 2;
						}
						TestString = realloc(TestString, capString * sizeof(char));
						ERROR_IFMALLOC(TestString);
				}
				sprintf(TestString, "%s%s%s", TestString, spaces, Buffer[idx]->testString);
#endif
		}
		ERROR_IFEQ(PEEK(), TERMINATE);
		identsexpressions_t* Expr = MALLOC(sizeof(identsexpressions_t));
		ERROR_IFMALLOC(Expr);
		Expr->Idents = MALLOC((idx+1)*sizeof(identexpr_t*));
		ERROR_IFMALLOC(Expr->Idents);
#ifdef ALLOW_TESTS
		Expr->testString = MALLOC((idxString+1) * sizeof(char));
		ERROR_IFMALLOC(Expr->testString);
#endif

		memcpy(Expr->Idents, Buffer, idx+1);
		free(Buffer);
		Buffer=NULL;

#ifdef ALLOW_TESTS
		memcpy(Expr->testString, TestString, idxString+1);
		free(TestString);
		TestString=NULL;
#endif

		return Expr;

}

funcexpr_t* ParseFuncExpr(token_t** Lexer, arena_t* Arena) {
		if (PEEK() == FUNC) {
				CONSUME();
		}
		funcexpr_t* Expr = MALLOC(sizeof(funcexpr_t));
		ERROR_IFMALLOC(Expr);
		ERROR_IFNEQ(PEEK(), L_PARENT);
		CONSUME();
		Expr->Idents = ParseMultipleIdents(Lexer, Arena, R_PARENT, COMMA);
		ERROR_IFFROM(Expr->Idents);
		ERROR_IFNEQ(PEEK(), R_PARENT);
		CONSUME();
		ERROR_IFNEQ(PEEK(), L_BRACKET);
		CONSUME();
		Expr->Block = ParseStatements(Lexer, Arena, R_BRACKET);
		ERROR_IFFROM(Expr->Block);
		ERROR_IFNEQ(PEEK(), R_BRACKET);
		CONSUME();
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Expr->Idents->testString) + STRLEN(Expr->Block->testString) + strlen("func () {  }");
		Expr->testString = MALLOC(len*sizeof(char));
		sprintf(Expr->testString, "func (%s) { %s }", Expr->Idents->testString, Expr->Block->testString);
#endif
		return Expr;
}

ifexpr_t* ParseIfExpr(token_t** Lexer, arena_t* Arena) {
		ifexpr_t* Expr = MALLOC(sizeof(ifexpr_t));
		ERROR_IFMALLOC(Expr);
		CONSUME();

		ERROR_IFNEQ(PEEK(), L_PARENT);
		CONSUME();

		Expr->Condition = ParseMultipleExpr(Lexer, Arena, R_PARENT);
		ERROR_IFFROM(Expr->Condition);

		ERROR_IFNEQ(PEEK(), R_PARENT);
		CONSUME();

		ERROR_IFNEQ(PEEK(), L_BRACKET);
		CONSUME();

		Expr->Consequence = ParseStatements(Lexer, Arena, R_BRACKET);
		ERROR_IFFROM(Expr->Consequence);

		ERROR_IFNEQ(PEEK(), R_BRACKET);
		CONSUME();

		if (PEEK() == ELSE) { 
				CONSUME();
				ERROR_IFNEQ(PEEK(), L_BRACKET);
				CONSUME();
				Expr->Alternative = ParseStatements(Lexer, Arena, R_BRACKET);
				ERROR_IFFROM(Expr->Alternative);
				ERROR_IFNEQ(PEEK(), R_BRACKET);
				CONSUME();
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Expr->Condition->testString) + strlen("if () {  } else {  }") + strlen(Expr->Consequence->testString) + strlen(Expr->Alternative->testString);
		Expr->testString = MALLOC(len*sizeof(char));
		ERROR_IFMALLOC(Expr->testString);
		sprintf(Expr->testString, "if (%s) { %s } else { %s }", Expr->Condition->testString, Expr->Consequence->testString, Expr->Alternative->testString);
#endif
		} 
#ifdef ALLOW_TESTS
		else {
				size_t len = STRLEN(Expr->Condition->testString) + strlen("if () {  }") + strlen(Expr->Consequence->testString);
				Expr->testString = MALLOC(len*sizeof(char));
				ERROR_IFMALLOC(Expr->testString);
				sprintf(Expr->testString, "if (%s) { %s }", Expr->Condition->testString, Expr->Consequence->testString);
		}
#endif
		return Expr;
}

static inline bool IsOperator(tokenType_t Operator) {
		switch (Operator) {
				case PLUS:
				case MINUS:
				case ASTERISK:
				case SLASH:
				case BOOL_EQUALS:
				case BOOL_NOT_EQUALS:
					return true;
		}
		return false;
};

expressions_t* ParseExpression(token_t** Lexer, arena_t* Arena, operators_priorities_t Priority) {
		expressions_t* Expression = MALLOC(sizeof(expressions_t));
		ERROR_IFMALLOC(Expression);
		// Prefix
		switch (PEEK()) {
				case FUNC:
						Expression->Expr = FUNC_EXPR;
						Expression->funcExpr = ParseFuncExpr(Lexer, Arena);
						ERROR_IFFROM(Expression->funcExpr);
#ifdef ALLOW_TESTS
						// No detection of error because if the testString is null, the expr will be returned as NULL
						Expression->testString = Expression->funcExpr->testString;
#endif
						break;
				case IF:
						Expression->Expr = IF_EXPR;
						Expression->ifExpr = ParseIfExpr(Lexer, Arena);
						ERROR_IFFROM(Expression->ifExpr);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->ifExpr->testString;
#endif
						break;
				case L_PARENT:
						CONSUME();
						Expression = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
						ERROR_IFFROM(Expression);
						ERROR_IFNEQ(PEEK(), R_PARENT);
						CONSUME();
						break;
				case MINUS:
				case BANG:
						Expression->Expr = PREFIX_EXPR;
						Expression->prefixExpr = ParsePrefixExpr(Lexer, Arena);
						ERROR_IFFROM(Expression->prefixExpr);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->prefixExpr->testString;
#endif
						break;
				case INT:
						Expression->Expr = INTEGER_EXPR;
						Expression->integerExpr = ParseIntExpr(Lexer, Arena);
						ERROR_IFFROM(Expression->integerExpr);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->integerExpr->testString;
#endif
						break;
				case IDENT:
						Expression->Expr = IDENT_EXPR;
						Expression->identExpr = ParseIdentExpr(Lexer, Arena);
						ERROR_IFFROM(Expression->identExpr);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->identExpr->testString;
#endif
						break;
				case STRING:
						Expression->Expr = STRING_EXPR;
						Expression->stringExpr = ParseStringExpr(Lexer, Arena);
						ERROR_IFFROM(Expression->stringExpr);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->stringExpr->testString;
#endif
						break;
				default:
						Expression->Expr = NONE_EXPR;
#ifdef ALLOW_TESTS
						Expression->testString = "";
#endif
						CONSUME();
						break;
		}
		// Infix
		while (IsOperator(PEEK())) {
				switch (PEEK()) {
						case BOOL_EQUALS:
						case BOOL_NOT_EQUALS:
								if (Priority > LOWEST_PRIOR) { goto end; }
						case PLUS:
						case MINUS:
								if (Priority >= SUM_PRIOR) { goto end; }
						case ASTERISK:
						case SLASH:
								if (Priority >= MULT_PRIOR) { goto end; }

								infixexpr_t* InfixExpr = ParseInfixExpr(Lexer, Arena, Expression);
								ERROR_IFFROM(InfixExpr);

								Expression = MALLOC(sizeof(expressions_t));
								ERROR_IFMALLOC(Expression);

								Expression->infixExpr = InfixExpr;
								Expression->Expr = INFIX_EXPR;
#ifdef ALLOW_TESTS
								Expression->testString = InfixExpr->testString;
#endif
								break;
				}
		}
end:
		return Expression;
}
