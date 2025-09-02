#include "parser.h"
#include "pMacros.h"
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "expressions.h"

exprstmt_t* ParseExprStmt(token_t** Lexer, arena_t* Arena) {
		exprstmt_t* Stmt = MALLOC(sizeof(statements_t));
		ERROR_IFMALLOC(Stmt);
		Stmt->Expr = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
		ERROR_IFFROM(Stmt->Expr);
		SKIPSEMI();
#ifdef ALLOW_TESTS
		Stmt->testString = Stmt->Expr->testString;
#endif
		return Stmt;
}

letstmt_t* ParseLetStmt(token_t** Lexer, arena_t* Arena) {
		letstmt_t* Stmt = MALLOC(sizeof(letstmt_t));
		ERROR_IFMALLOC(Stmt);
		ERROR_IFNEQ(PEEK(), IDENT);
		Stmt->Ident = ParseIdentExpr(Lexer, Arena);
		ERROR_IFFROM(Stmt->Ident);
		ERROR_IFNEQ(PEEK(), EQUAL);
		CONSUME();
		Stmt->Expr = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
		ERROR_IFFROM(Stmt->Expr);
		SKIPSEMI();
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Stmt->Expr->testString)+STRLEN(Stmt->Ident->testString)+strlen("let  = ");
		Stmt->testString = MALLOC(len*sizeof(char));
		ERROR_IFMALLOC(Stmt->testString);
		sprintf(Stmt->testString, "let %s = %s", Stmt->Ident->testString, Stmt->Expr->testString);
#endif
		return Stmt;
}

returnstmt_t* ParseReturnStmt(token_t** Lexer, arena_t* Arena) {
		returnstmt_t* Stmt = MALLOC(sizeof(returnstmt_t));
		ERROR_IFMALLOC(Stmt);
		Stmt->Expr = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
		ERROR_IFFROM(Stmt->Expr);
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Stmt->Expr->testString)+strlen("return ");
		Stmt->testString = MALLOC(len*sizeof(char));
		ERROR_IFMALLOC(Stmt->testString);
		sprintf(Stmt->testString, "return %s", Stmt->Expr->testString);
#endif
		SKIPSEMI();
		return Stmt;
}

funcstmt_t* ParseFuncStmt(token_t** Lexer, arena_t* Arena) {
		funcstmt_t* Stmt = MALLOC(sizeof(funcstmt_t));
		ERROR_IFMALLOC(Stmt);
		ERROR_IFNEQ(PEEK(), IDENT);
		Stmt->Ident = ParseIdentExpr(Lexer, Arena);
		ERROR_IFFROM(Stmt->Ident);
		Stmt->Expr = ParseFuncExpr(Lexer, Arena);
		ERROR_IFFROM(Stmt->Expr);
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Stmt->Expr->testString)+STRLEN(Stmt->Ident->testString);
		Stmt->testString = MALLOC(len*sizeof(char));
		ERROR_IFMALLOC(Stmt->testString);
		sprintf(Stmt->testString, "func %s%s", Stmt->Ident->testString, Stmt->Expr->testString+5);
#endif

		return Stmt;
}

statements_t *ParseStmt(token_t** Lexer, arena_t* Arena) {
		statements_t* Statement = MALLOC(sizeof(statements_t));
		ERROR_IFMALLOC(Statement);
#ifndef ALLOW_TESTS
#define CASES(stmt, caseNew, function) \
						Statement->Stmt = stmt; \
						Statement->caseNew = function(Lexer, Arena); \
						ERROR_IFFROM(Statement->caseNew); \
						break;
#else
#define CASES(stmt, caseNew, function) \
						Statement->Stmt = stmt; \
						Statement->caseNew = function(Lexer, Arena); \
						ERROR_IFFROM(Statement->caseNew); \
						Statement->testString = MALLOC((STRLEN(Statement->caseNew->testString)+strlen(";"))*sizeof(char)); \
						ERROR_IFMALLOC(Statement->testString); \
						sprintf(Statement->testString, "%s;", Statement->caseNew->testString); \
						break;
#endif
		ERROR_IFEQ(PEEK(), TERMINATE);
		switch (PEEK()) {
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
