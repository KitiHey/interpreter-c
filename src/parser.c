#include "ast.h"
#include "tokens.h"
#include "parser.h"
#include "arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PEEK() (*Lexer)->type
#define PEEK_NEXT() ((*Lexer)+1)->type
#define PEEK_LIT() (*Lexer)->literal
#define CONSUME() ((*Lexer)++)
#define SKIPSEMI() while (PEEK() == SEMICOLON) { CONSUME(); };
#define MALLOC(size) ArenaPush(Arena, size)
#define FREE(Arena) ArenaFree(Arena)
#define STRLEN(character) (strlen(character) + 1)

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
						free(TestString);
						return NULL;
				}
				Buffer[idx] = ParseIdentExpr(Lexer, Arena);
				if ( (PEEK() != spacing && PEEK() != delimiter) || Buffer[idx] == NULL) {
						printf("No coma, else: %s\n", PEEK_LIT());
						free(Buffer);
						free(TestString);
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

identexpr_t* ParseIdentExpr(token_t** Lexer, arena_t* Arena) {
		identexpr_t* Expr = MALLOC(sizeof(stringexpr_t));
		if (Expr == NULL) return NULL;

		Expr->Value = MALLOC(STRLEN(PEEK_LIT()) * sizeof(char) );
		if (Expr->Value == NULL) {
				return NULL;
		}

		strcat(Expr->Value, PEEK_LIT());
		free(PEEK_LIT());
		PEEK_LIT() = NULL;
#ifdef ALLOW_TESTS
		Expr->testString = MALLOC(STRLEN(Expr->Value) * sizeof(char));
		strcat(Expr->testString, Expr->Value);
#endif

		CONSUME();
		return Expr;
}

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
				return NULL;
		}

		strcat(Expr->Value, PEEK_LIT());
		free(PEEK_LIT());
		PEEK_LIT() = NULL;
#ifdef ALLOW_TESTS
		Expr->testString = MALLOC(( STRLEN(Expr->Value) + STRLEN("\"")*2) * sizeof(char));
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
		Expr->testString = MALLOC((STRLEN(Expr->RightExpr->testString)+strlen(Expr->Operator)+strlen("()")) * sizeof(char));
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
				case BOOL_EQUALS:
				case BOOL_NOT_EQUALS:
					return LOWEST_PRIOR;
		}
		return LOWEST_PRIOR;
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
		sprintf(Expr->testString, "(%s%s%s)", Expr->LeftExpr->testString, Expr->Operator, Expr->RightExpr->testString);
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
						Expression->testString = "";
						CONSUME();
						break;
		}
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

stmts_t *ParseStatements(token_t** Lexer, arena_t* Arena, tokenType_t delimiter) {
		SKIPSEMI();
		if (PEEK() == TERMINATE) {
				return NULL;
		};

		stmts_t *Stmt = MALLOC(sizeof(stmts_t));
		if (Stmt == NULL) {
				return NULL;
		}
#ifdef ALLOW_TESTS
		char* TestString = calloc(10,sizeof(char));
		if (TestString == NULL) { return NULL; }
		int capString = 10;
		int idxString = 0;
#endif
		statements_t** Statements = calloc(4, sizeof(expressions_t*));
		if (Statements == NULL) { return NULL; }
		int cap = 4;
		int idx = 0;
		for (idx = 0; PEEK() != TERMINATE && PEEK() != delimiter; idx++) {
				if (idx >= cap) {
						cap *= 2;
						Statements = realloc(Statements, cap*sizeof(statements_t*));
				}
				Statements[idx] = ParseStmt(Lexer, Arena);
				if (Statements[idx] == NULL) return NULL;
#ifdef ALLOW_TESTS
				idxString += strlen(Statements[idx]->testString)+1;
				if (idxString >= capString) {
						while (idxString >= capString) {
								capString *= 2;
						}
						TestString = realloc(TestString, capString * sizeof(char));
				}
				strcat(TestString, Statements[idx]->testString);
#endif
		}
		Stmt->Statements = MALLOC((idx+1)*sizeof(statements_t*));
		if (Stmt->Statements == NULL) { return NULL; }
#ifdef ALLOW_TESTS
		Stmt->testString = MALLOC((idxString+1) * sizeof(char));
		if (Stmt->testString == NULL) { return NULL; }
#endif

		memcpy(Stmt->Statements, Statements, idx+1);
		free(Statements);
		Statements=NULL;

#ifdef ALLOW_TESTS
		memcpy(Stmt->testString, TestString, idxString+1);
		free(TestString);
		TestString=NULL;
#endif

		return Stmt;
}

program_t Parser(token_t* Lexer) {
		program_t Program = {NULL};
		if (Lexer == NULL || Lexer[0].type == TERMINATE) return Program;

		Program.Arena = ArenaCreate(ARENA_CAP);
		if (Program.Arena == NULL) return Program;

		Program.Statements = ParseStatements(&Lexer, Program.Arena, TERMINATE);
		if (Program.Statements == NULL) {
				FREE(Program.Arena); 
				Program.Arena = NULL;
				return Program;
		}
#ifdef ALLOW_TESTS
		Program.testString = Program.Statements->testString;
#endif
		return Program;
}
