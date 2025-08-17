#include "ast.h"
#include "tokens.h"
#include "parser.h"
#include "arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PEEK() (*Lexer)->type
#define PEEK_LIT() (*Lexer)->literal
#define CONSUME() ((*Lexer)++)
#define SKIPSEMI() while (PEEK() == SEMICOLON) { CONSUME(); };
#define MALLOC(size) ArenaPush(Arena, size)
#define FREE(Arena) ArenaFree(Arena)
#define STRLEN(character) (strlen(character) + 1)

integerexpr_t* ParseIntExpr(token_t** Lexer, arena_t* Arena) {
		integerexpr_t* Expr = MALLOC(sizeof(integerexpr_t));
		if (Expr == NULL) return NULL;
		Expr->Value = atoi(PEEK_LIT());
#ifdef ALLOW_TESTS
		Expr->testString = MALLOC(STRLEN(PEEK_LIT()) * sizeof(char));
		sprintf(Expr->testString, "%s", PEEK_LIT());
#endif
		CONSUME();
		return Expr;
}

stringexpr_t* ParseStringExpr(token_t** Lexer, arena_t* Arena) {
		stringexpr_t* Expr = MALLOC(sizeof(stringexpr_t));
		if (Expr == NULL) return NULL;

		Expr->Value = MALLOC(STRLEN(PEEK_LIT()) * sizeof(char) );
		if (Expr->Value == NULL) {
				free(Expr);
				return NULL;
		}

		strcat(Expr->Value, PEEK_LIT());
		free(PEEK_LIT());
		PEEK_LIT() = NULL;
#ifdef ALLOW_TESTS
		Expr->testString = MALLOC(( STRLEN(Expr->Value) + strlen("\"")*2) * sizeof(char));
		sprintf(Expr->testString, "\"%s\"", Expr->Value);
#endif

		CONSUME();
		return Expr;
}

prefixexpr_t *ParsePrefixExpr(token_t** Lexer, arena_t* Arena) {
		prefixexpr_t *Expr = MALLOC(sizeof(prefixexpr_t));
		if (Expr == NULL) return NULL;

		Expr->Operator = MALLOC(STRLEN(PEEK_LIT()) * sizeof(char));
		strcat(Expr->Operator, PEEK_LIT());
		CONSUME();
		Expr->RightExpr = ParseExpression(Lexer, Arena, PREFIX_PRIOR);
#ifdef ALLOW_TESTS
		Expr->testString = malloc((STRLEN(Expr->RightExpr->testString)+strlen(Expr->Operator)*strlen("()")) * sizeof(char));
		sprintf(Expr->testString, "%c(%s)", Expr->Operator[0], Expr->RightExpr->testString);
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
		}
		return LOWEST_PRIOR;
}

static inline bool IsOperator(tokenType_t Operator) {
		switch (Operator) {
				case PLUS:
				case MINUS:
				case ASTERISK:
				case SLASH:
					return true;
		}
		return false;
}

infixexpr_t *ParseInfixExpr(token_t** Lexer, arena_t* Arena, expressions_t* leftExpr) {
		infixexpr_t *Expr = MALLOC(sizeof(infixexpr_t));
		if (Expr == NULL) return NULL;

		Expr->LeftExpr = leftExpr;
		Expr->Operator = MALLOC(STRLEN(PEEK_LIT()) * sizeof(char));
		strcat(Expr->Operator, PEEK_LIT());
		operators_priorities_t Priority = GetInfixPriority(PEEK());
		CONSUME();
		Expr->RightExpr = ParseExpression(Lexer, Arena, Priority);
#ifdef ALLOW_TESTS
		size_t len = STRLEN(Expr->RightExpr->testString)+strlen(Expr->LeftExpr->testString)+strlen(Expr->Operator)+strlen("()");
		Expr->testString = MALLOC(len * sizeof(char));
		sprintf(Expr->testString, "(%s%c%s)", Expr->LeftExpr->testString, Expr->Operator[0], Expr->RightExpr->testString);
#endif
		return Expr;
}

expressions_t* ParseExpression(token_t** Lexer, arena_t* Arena, operators_priorities_t Priority) {
		expressions_t* Expression = MALLOC(sizeof(expressions_t));
		if (Expression == NULL) return NULL;
		// Prefix
		switch (PEEK()) {
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
				case STRING:
						Expression->Expr = STRING_EXPR;
						Expression->stringExpr = ParseStringExpr(Lexer, Arena);
#ifdef ALLOW_TESTS
						Expression->testString = Expression->stringExpr->testString;
#endif
						break;
				default:
						Expression->Expr = NONE_EXPR;
						Expression->testString = "";
						CONSUME();
						break;
		}
		while (IsOperator(PEEK())) {
				switch (PEEK()) {
						case PLUS:
						case MINUS:
								if (Priority > SUM_PRIOR) { goto end; }
						case ASTERISK:
						case SLASH:
								if (Priority > MULT_PRIOR) {
										goto end;
								}
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

exprstmt_t* ParseExprStmt(token_t** Lexer, arena_t* Arena) {
		exprstmt_t* Stmt = MALLOC(sizeof(statements_t));
		if (Stmt == NULL) return NULL;
		Stmt->Expr = ParseExpression(Lexer, Arena, LOWEST_PRIOR);
		SKIPSEMI();
#ifdef ALLOW_TESTS
		Stmt->testString = Stmt->Expr->testString;
#endif
		return Stmt;
}

//letstmt_t* ParseLetStmt(token_t** Lexer, arena_t* Arena) {
//		letstmt_t* Stmt = MALLOC(sizeof(letstmt_t));
//		if (Stmt == NULL) return NULL;
//		return Stmt;
//}

statements_t *ParseStmt(token_t** Lexer, arena_t* Arena) {
		statements_t* Statement = MALLOC(sizeof(statements_t));
		if (Statement == NULL) { return NULL; }
		switch (PEEK()) {
				case TERMINATE:
						free(Statement);
						return NULL;
						//case LET:
						//		Statement->Stmt = LET_STMT;
						//		Statement->letStmt = ParseLetStmt(Lexer, Arena);
				default:
						Statement->Stmt = EXPR_STMT;
						Statement->exprStmt = ParseExprStmt(Lexer, Arena);
#ifdef ALLOW_TESTS
						Statement->testString = MALLOC((STRLEN(Statement->exprStmt->testString)+strlen(";"))*sizeof(char));
						sprintf(Statement->testString, "%s;", Statement->exprStmt->testString);
#endif
		}
		return Statement;
}

stmts_t *ParseStatements(token_t** Lexer, arena_t* Arena) {
		SKIPSEMI();
		if (PEEK() == TERMINATE) {
				return NULL;
		};

		stmts_t *Statements = MALLOC(sizeof(stmts_t));
		if (Statements == NULL) {
				FREE(Arena);
				return NULL;
		}
		Statements->Statement = ParseStmt(Lexer, Arena);
		Statements->Next = ParseStatements(Lexer, Arena);
#ifdef ALLOW_TESTS
		Statements->testString = Statements->Statement->testString;
#endif

		return Statements;
}

program_t Parser(token_t* Lexer) {
		program_t Program = {NULL};
		if (Lexer == NULL || Lexer[0].type == TERMINATE) return Program;

		Program.Arena = ArenaCreate(ARENA_CAP);
		if (Program.Arena == NULL) return Program;

		Program.Statements = ParseStatements(&Lexer, Program.Arena);
		if (Program.Statements == NULL) Program.Arena = NULL;
		return Program;
}
