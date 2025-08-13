#include "test.h"
#include "ast.h"
#include "tokens.h"
#include "parser.h"
#include "lexer.h"

typedef struct Test {
		int* times;
		int idx;
		char* input;
		char* expected;
} test_t;
#define IDX *(Test.times)

void TestFunc(test_t Test) {
		token_t *L = Lexer(Test.input);
		program_t P = Parser(L);
		nequal(P.Statements, NULL) {
				error(IDX, "Statements are NULL");
				return;
		}
		nequal(P.Statements[Test.idx], NULL) {
				error(IDX, "Statement[%d] is NULL", Test.idx);
				return;
		}
		equal(P.Statements[Test.idx]->Stmt, EXPR_STMT) {
				error(IDX, "Statement Isn't EXPR_STMT '%d'", P.Statements[Test.idx]->Stmt);
				return;
		}
		nequal(P.Statements[Test.idx]->exprStmt, NULL) {
				error(IDX, "Statement[%d] exprStmt is NULL", Test.idx);
				return;
		}
		nequal(P.Statements[Test.idx]->exprStmt->Expr, NULL) {
				error(IDX, "Statement[%d] exprStmt->expr is NULL", Test.idx);
				return;
		}
		equal(P.Statements[Test.idx]->exprStmt->Expr->Expr, STRING_EXPR) {
				error(IDX, "Expression Isn't STRING_EXPR '%d'", P.Statements[Test.idx]->exprStmt->Expr->Expr);
				return;
		}
		nequal(P.Statements[Test.idx]->exprStmt->Expr->stringExpr, NULL) {
				error(IDX, "Statement[%d] exprStmt->stringExpr is NULL", Test.idx);
				return;
		}
		equal((char*)P.Statements[Test.idx]->exprStmt->Expr->stringExpr->Value, Test.expected) {
				error(IDX, "STRING_EXPR isn't '%s', got '%d'", Test.expected, P.Statements[Test.idx]->exprStmt->Expr->stringExpr->Value);
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
		TestFunc( (test_t) {
						.input = "\"Hello\";\"World\"",
						.times = &times,
						.idx = 1,
						.expected = "World",
						}
				);
		TestFunc( (test_t) {
						.input = "\"Hello\";\"World\";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\"Tpe\"",
						.times = &times,
						.idx = 2,
						.expected = "Tpe",
						}
				);
		return 0;
}
