#pragma once
#include "tokens.h"
#include "arena.h"
#ifdef ALLOW_TESTS
#define TESTSTRING char* testString;
#else
#define TESTSTRING
#endif

// Fundamentals
typedef struct Statements statements_t;
typedef struct Expressions expressions_t;
typedef struct Stmts stmts_t;
typedef struct IdentExpr identexpr_t;

// Expressions
typedef enum ExpressionsTypes {
	NONE_EXPR,
	FUNC_EXPR,
	STRING_EXPR,
	INFIX_EXPR,
	PREFIX_EXPR,
	INTEGER_EXPR,
	IF_EXPR,
	IDENT_EXPR,
} expressionsTypes_t;

typedef enum OperatorsPriorities {
	LOWEST_PRIOR,
	SUM_PRIOR,
	MULT_PRIOR,
	PREFIX_PRIOR,
	HIGHEST_PRIOR,
} operators_priorities_t;

typedef struct ConditionExpressions {
	expressions_t** Expressions;
	TESTSTRING
} conditionexpressions_t;

typedef struct IdentsExpressions {
	identexpr_t** Idents;
	TESTSTRING
} identsexpressions_t;

typedef struct IfExpr {
	stmts_t* Consequence;
	stmts_t* Alternative;
	conditionexpressions_t* Condition;
	TESTSTRING
} ifexpr_t;

typedef struct FuncExpr {
	stmts_t* Block;
	identsexpressions_t* Idents;
	TESTSTRING
} funcexpr_t;

typedef struct PrefixExpr {
	expressions_t* RightExpr;
	char* Operator;
	TESTSTRING
} prefixexpr_t;

typedef struct InfixExpr {
	expressions_t* LeftExpr;
	expressions_t* RightExpr;
	char* Operator;
	TESTSTRING
} infixexpr_t;

typedef struct IdentExpr {
	char* Value;
	TESTSTRING
} identexpr_t;

typedef struct StringExpr {
	char* Value;
	TESTSTRING
} stringexpr_t;

typedef struct IntegerExpr {
	int Value;
	TESTSTRING
} integerexpr_t;

typedef struct Expressions {
	union {
			ifexpr_t *ifExpr;
			infixexpr_t *infixExpr;
			prefixexpr_t *prefixExpr;
			integerexpr_t *integerExpr;
			funcexpr_t *funcExpr;
			stringexpr_t *stringExpr;
			identexpr_t *identExpr;
	};
	expressionsTypes_t Expr;
	TESTSTRING
} expressions_t;

// Statements
typedef enum StatementsTypes {
	LET_STMT,
	RETURN_STMT,
	FUNC_STMT,
	EXPR_STMT,
} statementsTypes_t;

typedef struct FuncStmt {
	stmts_t* Block;
	identsexpressions_t* Idents;
	char* Ident;
	TESTSTRING
} funcstmt_t;

typedef struct ExprStmt {
	expressions_t* Expr;
	TESTSTRING
} exprstmt_t;

typedef struct ReturnStmt {
	expressions_t* Expr;
	TESTSTRING
} returnstmt_t;

typedef struct LetStmt {
	expressions_t* Expr;
	char* Ident;
	TESTSTRING
} letstmt_t;

typedef struct Statements {
		union {
				letstmt_t *letStmt;
				exprstmt_t *exprStmt;
				funcstmt_t *funcStmt;
				returnstmt_t *returnStmt;
		};
		statementsTypes_t Stmt;
		TESTSTRING
} statements_t;

typedef struct Stmts {
		statements_t** Statements;
		TESTSTRING
} stmts_t;

// Program
typedef struct Program {
		arena_t* Arena;
		stmts_t* Statements; // * Statements[]
		TESTSTRING
} program_t;
