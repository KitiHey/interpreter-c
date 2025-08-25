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
		if (Buffer == NULL) { return NULL; }
#ifdef ALLOW_TESTS
		char* TestString = calloc(10,  sizeof(char));
		if (TestString == NULL) { return NULL; }
		int capString = 10;
		int idxString = 0;
#endif
		int cap = 10;
		int idx = 0;
		for (idx = 0; PEEK() != delimiter && PEEK() != TERMINATE; idx++) {
				if (idx >= cap) {
						cap *= 2;
						Buffer = realloc(Buffer, cap*sizeof(expressions_t*));
				}
				Buffer[idx] = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
#ifdef ALLOW_TESTS
				idxString += strlen(Buffer[idx]->testString)+1;
				if (idxString >= capString) {
						while (idxString >= capString) {
								capString *= 2;
						}
						TestString = realloc(TestString, capString * sizeof(char));
				}
				strcat(TestString, Buffer[idx]->testString);
#endif
		}
		if (PEEK() == TERMINATE) { return NULL; }
		conditionexpressions_t* Expr = MALLOC(sizeof(conditionexpressions_t));
		if (Expr == NULL) { return NULL; }
		Expr->Expressions = MALLOC((idx+1)*sizeof(expressions_t*));
		if (Expr->Expressions == NULL) { return NULL; }
#ifdef ALLOW_TESTS
		Expr->testString = MALLOC((idxString+1) * sizeof(char));
		if (Expr->testString == NULL) { return NULL; }
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
		if (Buffer == NULL) { return NULL; }
#ifdef ALLOW_TESTS
		char* TestString = calloc(10,  sizeof(char));
		if (TestString == NULL) { return NULL; }
		int capString = 10;
		int idxString = 0;
#endif
		int cap = 10;
		int idx = 0;
		for (idx = 0; PEEK() != delimiter && PEEK() != TERMINATE; idx++) {
				if (idx >= cap) {
						cap *= 2;
						Buffer = realloc(Buffer, cap*sizeof(identexpr_t*));
				}
				if (PEEK() != IDENT) {
						printf("No ident, else: %s\n", PEEK_LIT());
						free(Buffer);
#ifdef ALLOW_TESTS
						free(TestString);
#endif
						return NULL;
				}
				Buffer[idx] = ParseIdentExpr(Lexer, Arena);
				if ( (PEEK() != spacing && PEEK() != delimiter) || Buffer[idx] == NULL) {
						printf("No coma, else: %s\n", PEEK_LIT());
						free(Buffer);
#ifdef ALLOW_TESTS
						free(TestString);
#endif
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
				}
				sprintf(TestString, "%s%s%s", TestString, spaces, Buffer[idx]->testString);
#endif
		}
		if (PEEK() == TERMINATE) { return NULL; }
		identsexpressions_t* Expr = MALLOC(sizeof(identsexpressions_t));
		if (Expr == NULL) { return NULL; }
		Expr->Idents = MALLOC((idx+1)*sizeof(identexpr_t*));
		if (Expr->Idents == NULL) { return NULL; }
#ifdef ALLOW_TESTS
		Expr->testString = MALLOC((idxString+1) * sizeof(char));
		if (Expr->testString == NULL) { return NULL; }
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
		if (Expr == NULL) { return NULL; }
		if (PEEK() != L_PARENT) { return NULL; }
		CONSUME();
		Expr->Idents = ParseMultipleIdents(Lexer, Arena, R_PARENT, COMMA);
		if (Expr->Idents == NULL) { return NULL; }
		if (PEEK() != R_PARENT) { return NULL; }
		CONSUME();
		if (PEEK() != L_BRACKET) { return NULL; }
		CONSUME();
		Expr->Block = ParseStatements(Lexer, Arena, R_BRACKET);
		if (PEEK() != R_BRACKET) { return NULL; }
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
		if (Expr == NULL) { return NULL; }
		CONSUME();

		if (PEEK() != L_PARENT) { return NULL; }
		CONSUME();

		Expr->Condition = ParseMultipleExpr(Lexer, Arena, R_PARENT);
		if (Expr->Condition == NULL) { return NULL; }

		if (PEEK() != R_PARENT) { return NULL; }
		CONSUME();
		if (PEEK() != L_BRACKET) { return NULL; }
		CONSUME();
		Expr->Consequence = ParseStatements(Lexer, Arena, R_BRACKET);
		if (PEEK() != R_BRACKET) { return NULL; }
		CONSUME();

		if (PEEK() == ELSE) { 
				CONSUME();
				if (PEEK() != L_BRACKET) { return NULL; }
				CONSUME();
				Expr->Alternative = ParseStatements(Lexer, Arena, R_BRACKET);
				if (PEEK() != R_BRACKET) { return NULL; }
				CONSUME();
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Expr->Condition->testString) + strlen("if () {  } else {  }") + strlen(Expr->Consequence->testString) + strlen(Expr->Alternative->testString);
		Expr->testString = MALLOC(len*sizeof(char));
		sprintf(Expr->testString, "if (%s) { %s } else { %s }", Expr->Condition->testString, Expr->Consequence->testString, Expr->Alternative->testString);
#endif
		} 
#ifdef ALLOW_TESTS
		else {

				size_t len = STRLEN(Expr->Condition->testString) + strlen("if () {  }") + strlen(Expr->Consequence->testString);
				Expr->testString = MALLOC(len*sizeof(char));
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
		if (Expression == NULL) return NULL;
		// Prefix
		switch (PEEK()) {
				case FUNC:
						Expression->Expr = FUNC_EXPR;
						Expression->funcExpr = ParseFuncExpr(Lexer, Arena);
						if (Expression->funcExpr == NULL) { return NULL; }
#ifdef ALLOW_TESTS
						Expression->testString = Expression->funcExpr->testString;
#endif
						break;
				case IF:
						Expression->Expr = IF_EXPR;
						Expression->ifExpr = ParseIfExpr(Lexer, Arena);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->ifExpr->testString;
#endif
						break;
				case L_PARENT:
						CONSUME();
						Expression = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
						if (PEEK() != R_PARENT) {
								return NULL;
						}
						CONSUME();
						break;
				case MINUS:
				case BANG:
						Expression->Expr = PREFIX_EXPR;
						Expression->prefixExpr = ParsePrefixExpr(Lexer, Arena);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->prefixExpr->testString;
#endif
						break;
				case INT:
						Expression->Expr = INTEGER_EXPR;
						Expression->integerExpr = ParseIntExpr(Lexer, Arena);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->integerExpr->testString;
#endif
						break;
				case IDENT:
						Expression->Expr = IDENT_EXPR;
						Expression->identExpr = ParseIdentExpr(Lexer, Arena);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->identExpr->testString;
#endif
						break;
				case STRING:
						Expression->Expr = STRING_EXPR;
						Expression->stringExpr = ParseStringExpr(Lexer, Arena);
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
								Expression = MALLOC(sizeof(expressions_t));
								if (Expression == NULL) { return NULL; }
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
