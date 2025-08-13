#include "test.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"

int main() {
	token_t *L = Lexer(";49;");
	program_t P = Parser(L);
	nequal(P.Statements, NULL) {
		error(0, "'Statements' is NULL");
		return 1;
	}
	nequal(P.Statements[0], NULL) {
		error(0, "'Statements[0]' is NULL");
		return 1;
	}
	equal(P.Statements[1], NULL) {
		error(0, "'Statements[1]' isn't NULL");
		return 1;
	}
	equal(P.Statements[0]->Stmt, EXPR_STMT) {
			error(0, "'Statement[0]' isn't EXPR_STMT: %d", P.Statements[0]->Stmt);
			return 1;
	}
	nequal(P.Statements[0]->exprStmt->Expr, NULL) {
			error(0, "'Statements[0] Expr' is NULL");
			return 1;
	}
	equal(P.Statements[0]->exprStmt->Expr->Expr, INTEGER_EXPR) {
			error(0, "'Statement[0] Expr' isn't INTEGER_EXPR: %d", P.Statements[0]->exprStmt->Expr->Expr);
			return 1;
	}
	nequal(P.Statements[0]->exprStmt->Expr->integerExpr, NULL) {
			error(0, "'Statements[0] intergerExpr' is NULL");
			return 1;
	}
	iequal(P.Statements[0]->exprStmt->Expr->integerExpr->Value, 49) {
			error(0, "'Statement 0 expr' isn't 49: %d", P.Statements[0]->exprStmt->Expr->integerExpr->Value);
			return 1;
	}
	success(0, "Got 49!");
	return 0;
}
