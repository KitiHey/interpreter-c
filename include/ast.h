#include "tokens.h"

// Fundamentals
typedef struct Statements statements_t;
typedef struct Expressions expressions_t;

// Expressions
typedef enum ExpressionsTypes {
	FUNC_EXPR,
	STRING_EXPR,
	MATH_EXPR,
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
	expressions_t* Condition;
	statements_t* Consequence;
	statements_t* Alternative;
} ifexpr_t

typedef struct FuncExpr {
	statements_t* Block;
	expressions_t* Condition;
} funcexpr_t;

typedef struct MathExpr {
	expressions_t Left;
	operators_t Operator;
	expressions_t Right;
} mathexpr_t;

typedef struct StringExpr {
	char* Value;
} stringexpr_t;

typedef struct IntegerExpr {
	int Value;
} integerexpr_t;

typedef struct Expressions {
	expressionsTypes_t Expr;
	union {
			ifexpr_t ifExpr;
			mathexpr_t mathExpr;
			integerexpr_t integerExpr;
			funcexpr_t funcExpr;
			stringexpr_t stringExpr;
	}
} expressions_t;

// Statements
typedef enum StatementsTypes {
	LET_STMT,
	RETURN_STMT,
	FUNC_STMT,
	EXPR_STMT,
} statementsTypes_t;

typedef struct FuncStmt {
	char* Ident;
	statements_t* Block;
	expressions_t* Condition;
} funcstmt_t;

typedef struct ExprStmt {
	Expressions Expr;
} exprstmt_t;

typedef struct ReturnStmt {
	Expressions Expr;
} returnstmt_t;

typedef struct LetStmt {
	char* Ident;
	Expressions Expr;
} letstmt_t;

typedef struct Statements {
		statementsTypes_t Stmt;
		union {
				letstmt_t letStmt;
				exprstmt_t exprStmt;
				funcstmt_t funcStmt;
				returnstmt_t returnStmt;	
		};
} statements_t;

// Program
typedef struct Program {
		statements_t* statements;
} program_t;
