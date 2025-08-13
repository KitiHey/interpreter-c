#pragma once
#include "tokens.h"
#include "arena.h"

// Fundamentals
typedef struct Statements statements_t;
typedef struct Expressions expressions_t;

// Expressions
typedef enum ExpressionsTypes {
	NONE_EXPR,
	FUNC_EXPR,
	STRING_EXPR,
	INFIX_EXPR,
	PREFIX_EXPR,
	INTEGER_EXPR,
	IF_EXPR,
} expressionsTypes_t;

typedef enum Operators {
	UNEXISTENT,
	SUM,
	MULT,
	REST,
	DIV,
} operators_t;

typedef struct IfExpr {
	statements_t** Consequence;
	statements_t** Alternative;
	expressions_t** Condition;
} ifexpr_t;

typedef struct FuncExpr {
	statements_t** Block;
	expressions_t** Condition;
} funcexpr_t;

typedef struct PrefixExpr {
	expressions_t* LeftExpr;
	expressions_t* RightExpr;
	operators_t Operator;
} prefixexpr_t;

typedef struct InfixExpr {
	expressions_t* LeftExpr;
	expressions_t* RightExpr;
	operators_t Operator;
} infixexpr_t;

typedef struct StringExpr {
	char Value[];
} stringexpr_t;

typedef struct IntegerExpr {
	int Value;
} integerexpr_t;

typedef struct Expressions {
	union {
			ifexpr_t *ifExpr;
			infixexpr_t *infixExpr;
			prefixexpr_t *prefixExpr;
			integerexpr_t *integerExpr;
			funcexpr_t *funcExpr;
			stringexpr_t *stringExpr;
	};
	expressionsTypes_t Expr;
} expressions_t;

// Statements
typedef enum StatementsTypes {
	LET_STMT,
	RETURN_STMT,
	FUNC_STMT,
	EXPR_STMT,
} statementsTypes_t;

typedef struct FuncStmt {
	statements_t** Block;
	expressions_t** Condition;
	char Ident[];
} funcstmt_t;

typedef struct ExprStmt {
	expressions_t* Expr;
} exprstmt_t;

typedef struct ReturnStmt {
	expressions_t* Expr;
} returnstmt_t;

typedef struct LetStmt {
	expressions_t* Expr;
	char Ident[];
} letstmt_t;

typedef struct Statements {
		union {
				letstmt_t *letStmt;
				exprstmt_t *exprStmt;
				funcstmt_t *funcStmt;
				returnstmt_t *returnStmt;
		};
		statementsTypes_t Stmt;
} statements_t;

// Program
typedef struct Program {
		arena_t* Arena;
		statements_t** Statements; // * Statements[]
} program_t;
