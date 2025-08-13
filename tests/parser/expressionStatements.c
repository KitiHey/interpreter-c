#include "test.h"
#include "ast.h"
#include "tokens.h"
#include "parser.h"
#include "lexer.h"

typedef struct Test {
		int* times;
		char* input;
		int idx;
		char* expected;
} test_t;
#define IDX *(Test.times)

void TestFuncNext(test_t Test) {
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
		equal((char*)P.Statements->Next->Statement->exprStmt->Expr->stringExpr->Value, Test.expected) {
				error(IDX, "STRING_EXPR isn't '%s', got '%d'", Test.expected, P.Statements->Next->Statement->exprStmt->Expr->stringExpr->Value);
				return;
		}
		success(IDX, "Got %s", Test.expected);
}

void TestFunc(test_t Test) {
		token_t *L = Lexer(Test.input);
		program_t P = Parser(L);
		nequal(P.Statements, NULL) {
				error(IDX, "Statements are NULL");
				return;
		}
		nequal(P.Statements->Statement, NULL) {
				error(IDX, "Statement[%d] is NULL", Test.idx);
				return;
		}
		equal(P.Statements->Statement->Stmt, EXPR_STMT) {
				error(IDX, "Statement Isn't EXPR_STMT '%d'", P.Statements->Statement->Stmt);
				return;
		}
		nequal(P.Statements->Statement->exprStmt, NULL) {
				error(IDX, "Statement[%d] exprStmt is NULL", Test.idx);
				return;
		}
		nequal(P.Statements->Statement->exprStmt->Expr, NULL) {
				error(IDX, "Statement[%d] exprStmt->expr is NULL", Test.idx);
				return;
		}
		equal(P.Statements->Statement->exprStmt->Expr->Expr, STRING_EXPR) {
				error(IDX, "Expression Isn't STRING_EXPR '%d'", P.Statements->Statement->exprStmt->Expr->Expr);
				return;
		}
		nequal(P.Statements->Statement->exprStmt->Expr->stringExpr, NULL) {
				error(IDX, "Statement[%d] exprStmt->stringExpr is NULL", Test.idx);
				return;
		}
		equal((char*)P.Statements->Statement->exprStmt->Expr->stringExpr->Value, Test.expected) {
				error(IDX, "STRING_EXPR isn't '%s', got '%d'", Test.expected, P.Statements->Statement->exprStmt->Expr->stringExpr->Value);
				return;
		}
		success(IDX, "Got %s", Test.expected);
}

int main() {
		int times = 0;
		TestFunc( (test_t) {
						.input = "\"Hello\";\"World\"",
						.times = &times,
						.idx = 0,
						.expected = "Hello",
						}
				);
		TestFuncNext( (test_t) {
						.input = "\"Hello\";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\"World\"",
						.times = &times,
						.idx = 1,
						.expected = "World",
						}
				);
		return 0;
}
