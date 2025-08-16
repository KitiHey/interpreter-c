#include "test.h"
#include "ast.h"
#include "tokens.h"
#include "parser.h"
#include "lexer.h"
#include <stdbool.h>

typedef struct Test {
		int* times;
		char* input;
		int idx;
		void* expected;
} test_t;
#define IDX *(Test.times)

void TestFuncStringNext(test_t Test) {
		token_t *L = Lexer(Test.input);
		program_t P = Parser(L);
		nequal(P.Statements, NULL) {
				error(IDX, "Statements are NULL");
				return;
		}
		nequal(P.Statements->Next->Statement, NULL) {
				error(IDX, "Statement[%d] is NULL", Test.idx);
				return;
		}
		equal(P.Statements->Next->Statement->Stmt, EXPR_STMT) {
				error(IDX, "Statement Isn't EXPR_STMT '%d'", P.Statements->Next->Statement->Stmt);
				return;
		}
		nequal(P.Statements->Next->Statement->exprStmt, NULL) {
				error(IDX, "Statement[%d] exprStmt is NULL", Test.idx);
				return;
		}
		nequal(P.Statements->Next->Statement->exprStmt->Expr, NULL) {
				error(IDX, "Statement[%d] exprStmt->expr is NULL", Test.idx);
				return;
		}
		equal(P.Statements->Next->Statement->exprStmt->Expr->Expr, STRING_EXPR) {
				error(IDX, "Expression Isn't STRING_EXPR '%d'", P.Statements->Next->Statement->exprStmt->Expr->Expr);
				return;
		}
		nequal(P.Statements->Next->Statement->exprStmt->Expr->stringExpr, NULL) {
				error(IDX, "Statement[%d] exprStmt->stringExpr is NULL", Test.idx);
				return;
		}
		equal((char*)P.Statements->Next->Statement->exprStmt->Expr->stringExpr->Value, (char*)Test.expected) {
				error(IDX, "STRING_EXPR isn't '%s', got '%d'", (char*)Test.expected, P.Statements->Next->Statement->exprStmt->Expr->stringExpr->Value);
				return;
		}
		success(IDX, "Got %s", Test.expected);
}

bool TestFuncExprStatement(test_t Test, token_t *L, program_t P ) {
		nequal(P.Statements, NULL) {           
				error(IDX, "Statements are NULL");
				return false;
		}
		nequal(P.Statements->Statement, NULL) {
				error(IDX, "Statement[%d] is NULL", Test.idx);
				return false;
		}
		equal(P.Statements->Statement->Stmt, EXPR_STMT) {
				error(IDX, "Statement Isn't EXPR_STMT '%d'", P.Statements->Statement->Stmt);
				return false;
		}
		nequal(P.Statements->Statement->exprStmt, NULL) {
				error(IDX, "Statement[%d] exprStmt is NULL", Test.idx);
				return false;
		}
		nequal(P.Statements->Statement->exprStmt->Expr, NULL) {
				error(IDX, "Statement[%d] exprStmt->expr is NULL", Test.idx);
				return false;
		}
		return true;
}

void TestFuncString(test_t Test) {
		token_t *L = Lexer(Test.input);
		program_t P = Parser(L);
		if (!TestFuncExprStatement(Test, L, P)) { return; }
		equal(P.Statements->Statement->exprStmt->Expr->Expr, STRING_EXPR) {
				error(IDX, "Expression Isn't STRING_EXPR '%d'", P.Statements->Statement->exprStmt->Expr->Expr);
				return;
		}
		nequal(P.Statements->Statement->exprStmt->Expr->stringExpr, NULL) {
				error(IDX, "Statement[%d] exprStmt->stringExpr is NULL", Test.idx);
				return;
		}
		equal((char*)P.Statements->Statement->exprStmt->Expr->stringExpr->Value, (char*)Test.expected) {
				error(IDX, "STRING_EXPR isn't '%s', got '%d'", (char*)Test.expected, P.Statements->Statement->exprStmt->Expr->stringExpr->Value);
				return;
		}
		success(IDX, "Got %s", Test.expected);
}

int main() {
		int times = 0;
		TestFuncString( (test_t) {
						.input = "\"Hello\";\"World\"",
						.times = &times,
						.idx = 0,
						.expected = "Hello",
						}
				);
		TestFuncStringNext( (test_t) {
						.input = "\"Hello\";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\"World\"",
						.times = &times,
						.idx = 1,
						.expected = "World",
						}
				);
		return 0;
}
