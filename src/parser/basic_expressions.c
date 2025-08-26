#include "expressions.h"
#include "parser.h"
#include "ast.h"
#include "tokens.h"
#include "arena.h"
#include "pMacros.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

operators_priorities_t GetInfixPriority(tokenType_t Operator);

identexpr_t* ParseIdentExpr(token_t** Lexer, arena_t* Arena) {
		identexpr_t* Expr = MALLOC(sizeof(stringexpr_t));
		if (Expr == NULL) return NULL;

		Expr->Value = STRDUP(PEEK_LIT());
		if (Expr->Value == NULL) return NULL;
#ifdef ALLOW_TESTS
		Expr->testString = Expr->Value;
#endif
		CONSUME();
		return Expr;
}

integerexpr_t* ParseIntExpr(token_t** Lexer, arena_t* Arena) {
		integerexpr_t* Expr = MALLOC(sizeof(integerexpr_t));
		if (Expr == NULL) return NULL;
		Expr->Value = atoi(PEEK_LIT());

#ifdef ALLOW_TESTS
		size_t len = STRLEN(PEEK_LIT());
		Expr->testString = MALLOC(len * sizeof(char));
		if (Expr->testString == NULL) return NULL;

		sprintf(Expr->testString, "%d", Expr->Value);
#endif
		CONSUME();
		return Expr;
}

stringexpr_t* ParseStringExpr(token_t** Lexer, arena_t* Arena) {
		stringexpr_t* Expr = MALLOC(sizeof(stringexpr_t));
		if (Expr == NULL) return NULL;

		Expr->Value = STRDUP(PEEK_LIT());
		if (Expr->Value == NULL) return NULL;
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Expr->Value) + STRLEN("\"")*2;
		Expr->testString = MALLOC(len * sizeof(char));
		if (Expr->testString == NULL) return NULL;

		sprintf(Expr->testString, "\"%s\"", Expr->Value);
#endif

		CONSUME();
		return Expr;
}

prefixexpr_t *ParsePrefixExpr(token_t** Lexer, arena_t* Arena) {
		prefixexpr_t *Expr = MALLOC(sizeof(prefixexpr_t));
		if (Expr == NULL) return NULL;

		Expr->Operator = STRDUP(PEEK_LIT());
		if (Expr->Operator == NULL) return NULL;

		CONSUME();
		Expr->RightExpr = ParseExpression(Lexer, Arena, PREFIX_PRIOR);
		if (Expr->RightExpr == NULL) return NULL;
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Expr->RightExpr->testString)+strlen(Expr->Operator)+strlen("()");
		Expr->testString = MALLOC(len * sizeof(char));
		if (Expr->testString == NULL) return NULL;

		sprintf(Expr->testString, "%c(%s)", Expr->Operator[0], Expr->RightExpr->testString);
#endif
		return Expr;
}

infixexpr_t *ParseInfixExpr(token_t** Lexer, arena_t* Arena, expressions_t* leftExpr) {
		infixexpr_t *Expr = MALLOC(sizeof(infixexpr_t));
		if (Expr == NULL) return NULL;

		Expr->LeftExpr = leftExpr;
		Expr->Operator = STRDUP(PEEK_LIT());
		if (Expr->Operator == NULL) return NULL;

		operators_priorities_t Priority = GetInfixPriority(PEEK());
		CONSUME();
		Expr->RightExpr = ParseExpression(Lexer, Arena, Priority);
		if (Expr->RightExpr == NULL) return NULL;
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Expr->RightExpr->testString)+strlen(Expr->LeftExpr->testString)+strlen(Expr->Operator)+strlen("()");
		Expr->testString = MALLOC(len * sizeof(char));
		if (Expr->testString == NULL) return NULL;

		sprintf(Expr->testString, "(%s%s%s)", Expr->LeftExpr->testString, Expr->Operator, Expr->RightExpr->testString);
#endif
		return Expr;
}

operators_priorities_t GetInfixPriority(tokenType_t Operator) {
		switch (Operator) {
				case PLUS:
				case MINUS:
					return SUM_PRIOR;
				case ASTERISK:
				case SLASH:
					return MULT_PRIOR;
				case BOOL_EQUALS:
				case BOOL_NOT_EQUALS:
					return LOWEST_PRIOR;
		}
		return LOWEST_PRIOR;
}
