#include "parser.h"
#include "pMacros.h"
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "expressions.h"

exprstmt_t* ParseExprStmt(token_t** Lexer, arena_t* Arena) {
		exprstmt_t* Stmt = MALLOC(sizeof(statements_t));
		if (Stmt == NULL) return NULL;
		Stmt->Expr = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
		if (Stmt->Expr == NULL) return NULL;
		SKIPSEMI();
#ifdef ALLOW_TESTS
		Stmt->testString = Stmt->Expr->testString;
#endif
		return Stmt;
}

letstmt_t* ParseLetStmt(token_t** Lexer, arena_t* Arena) {
		letstmt_t* Stmt = MALLOC(sizeof(letstmt_t));
		if (Stmt == NULL) return NULL;
		if (PEEK() != IDENT) { return NULL; }
		Stmt->Ident = ParseIdentExpr(Lexer, Arena);
		if (Stmt->Ident == NULL) { return NULL; }
		if (PEEK() != EQUAL) { return NULL; }
		CONSUME();
		Stmt->Expr = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
		if (Stmt->Expr == NULL) { return NULL; }
		SKIPSEMI();
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Stmt->Expr->testString)+STRLEN(Stmt->Ident->testString)+strlen("let  = ");
		Stmt->testString = MALLOC(len*sizeof(char));
		sprintf(Stmt->testString, "let %s = %s", Stmt->Ident->testString, Stmt->Expr->testString);
#endif
		return Stmt;
}

returnstmt_t* ParseReturnStmt(token_t** Lexer, arena_t* Arena) {
		returnstmt_t* Stmt = MALLOC(sizeof(returnstmt_t));
		if (Stmt == NULL) { return NULL; }
		Stmt->Expr = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
		if (Stmt->Expr == NULL) { return NULL; }
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Stmt->Expr->testString)+strlen("return ");
		Stmt->testString = MALLOC(len*sizeof(char));
		sprintf(Stmt->testString, "return %s", Stmt->Expr->testString);
#endif
		SKIPSEMI();
		return Stmt;
}

funcstmt_t* ParseFuncStmt(token_t** Lexer, arena_t* Arena) {
		funcstmt_t* Stmt = MALLOC(sizeof(funcstmt_t));
		if (Stmt == NULL) { return NULL; }
		if (PEEK() != IDENT) { return NULL; }
		Stmt->Ident = ParseIdentExpr(Lexer, Arena);
		if (Stmt->Ident == NULL) { return NULL; }
		Stmt->Expr = ParseFuncExpr(Lexer, Arena);
		if (Stmt->Expr == NULL) { return NULL; }
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Stmt->Expr->testString)+STRLEN(Stmt->Ident->testString);
		Stmt->testString = MALLOC(len*sizeof(char));
		sprintf(Stmt->testString, "func %s%s", Stmt->Ident->testString, Stmt->Expr->testString+5);
#endif

		return Stmt;
}

statements_t *ParseStmt(token_t** Lexer, arena_t* Arena) {
		statements_t* Statement = MALLOC(sizeof(statements_t));
#ifndef ALLOW_TESTS
#define CASES(stmt, caseNew, function) \
						Statement->Stmt = stmt; \
						Statement->caseNew = function(Lexer, Arena); \
						if (Statement->caseNew == NULL) { return NULL; } \
						break;
#else
#define CASES(stmt, caseNew, function) \
						Statement->Stmt = stmt; \
						Statement->caseNew = function(Lexer, Arena); \
						if (Statement->caseNew == NULL) { return NULL; } \
						Statement->testString = MALLOC((STRLEN(Statement->caseNew->testString)+strlen(";"))*sizeof(char)); \
						if (Statement->testString == NULL) { return NULL; } \
						sprintf(Statement->testString, "%s;", Statement->caseNew->testString); \
						break;
#endif
		if (Statement == NULL) { return NULL; }
		switch (PEEK()) {
				case TERMINATE:
						return NULL;
				case FUNC:
						if (PEEK_NEXT() == IDENT) {
								CONSUME();
								CASES(FUNC_STMT, funcStmt, ParseFuncStmt);
								break;
						}
						goto parseExprStmt;
						break;
				case RETURN:
						CONSUME();
						CASES(RETURN_STMT, returnStmt, ParseReturnStmt);
						break;
				case LET:
						CONSUME();
						CASES(LET_STMT, letStmt, ParseLetStmt);
						break;
parseExprStmt:
				default:
						CASES(EXPR_STMT, exprStmt, ParseExprStmt);
						break;
		}
#undef CASES
		SKIPSEMI();
		return Statement;
}
