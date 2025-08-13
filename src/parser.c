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

integerexpr_t* ParseIntExpr(token_t** Lexer, arena_t* Arena) {
		integerexpr_t* Expr = ArenaPush(Arena, sizeof(integerexpr_t));
		if (Expr == NULL) return NULL;
		Expr->Value = atoi(PEEK_LIT());
		CONSUME();
		return Expr;
}

stringexpr_t* ParseStringExpr(token_t** Lexer, arena_t* Arena) {
		stringexpr_t* Expr = ArenaPush(Arena, sizeof(stringexpr_t));
		if (Expr == NULL) return NULL;

		size_t len = strlen( PEEK_LIT() );
		Expr->Value = ArenaPush(Arena, ( len+1 ) * sizeof(char*) );
		if (Expr->Value == NULL) { 
				free(Expr);
				return NULL;
		}

		strcat(Expr->Value, PEEK_LIT());
		free(PEEK_LIT());
		PEEK_LIT() = NULL;

		CONSUME();
		return Expr;
}

expressions_t* ParseExpression(token_t** Lexer, arena_t* Arena) {
		expressions_t* Expression = ArenaPush(Arena, sizeof(expressions_t));
		if (Expression == NULL) { return NULL; }
		// Prefix
		switch (PEEK()) {
				case INT:
						Expression->Expr = INTEGER_EXPR;
						Expression->integerExpr = ParseIntExpr(Lexer, Arena);
						break;
				case STRING:
						Expression->Expr = STRING_EXPR;
						Expression->stringExpr = ParseStringExpr(Lexer, Arena);
						break;
				default:
						Expression->Expr = NONE_EXPR;
						CONSUME();
						break;
		}
		return Expression;
}

exprstmt_t* ParseExprStmt(token_t** Lexer, arena_t* Arena) {
		exprstmt_t* Stmt = ArenaPush(Arena, sizeof(statements_t));
		if (Stmt == NULL) return NULL;
		Stmt->Expr = ParseExpression(Lexer, Arena);
		SKIPSEMI();
		return Stmt;
}

//letstmt_t* ParseLetStmt(token_t** Lexer, arena_t* Arena) {
//		letstmt_t* Stmt = ArenaPush(Arena, sizeof(letstmt_t));
//		if (Stmt == NULL) return NULL;
//		return Stmt;
//}

statements_t *ParseStatements(token_t** Lexer, arena_t* Arena) {
	statements_t* Statement = ArenaPush(Arena, sizeof(statements_t));
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
	}
	return Statement;
}

program_t Parser(token_t* Lexer) {
	program_t Program = {NULL};
	if (Lexer == NULL || Lexer[0].type == TERMINATE) return Program;
	Program.Arena = ArenaCreate(1000000);
	if (Program.Arena == NULL) return Program;

	Program.Statements = malloc(2*sizeof(statements_t*));
	if (Program.Statements == NULL) { 
			ArenaFree(Program.Arena);
			return Program; 
	}

	size_t size = 2;
	size_t idx = 0;
	while ( (*Lexer).type != TERMINATE ) {
			if (idx+1 >= size) {
					size*=2;
					Program.Statements = realloc(Program.Statements, size*sizeof(statements_t*));
					if (Program.Statements == NULL) { 
							ArenaFree(Program.Arena);
							return Program;
					}
			}
			if ((*Lexer).type == SEMICOLON) {
					Lexer++;
					continue;
			}
			Program.Statements[idx++] = ParseStatements(&Lexer, Program.Arena);
	}
	return Program;
}

