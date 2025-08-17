#include "test.h"
#include "parser.h"
#include "lexer.h"
#include "ast.h"

#define TEST_(...) Test( (tests_t) { Idx: &Idx, __VA_ARGS__ } );

typedef struct Tests {
	char* Expected;
	char* Input;
	int StmtIdx;
	int* Idx;
} tests_t;

void Test(tests_t testT) {
		token_t *L = Lexer(testT.Input);
		program_t P = Parser(L);

		stmts_t *Stmt = P.Statements;
		nequal(Stmt, NULL) {
				error(*(testT.Idx), "stmt 0 is NULL");
				return;
		}
		for (int i = 1; i <= testT.StmtIdx; i++) {
				Stmt = Stmt->Next;
				nequal(Stmt, NULL) {
						error(*(testT.Idx), "stmt %d is NULL", i);
						return;
				}
		}
		statements_t *Statement = Stmt->Statement;
		equal(Statement->testString, testT.Expected) {
				error(*(testT.Idx), "got '%s' instead of '%s'", Statement->testString, testT.Expected);
				return;
		}
		success(*(testT.Idx), "got %s", testT.Expected);
		*(testT.Idx)++;
}

int main() {
	int Idx = 0;
	TEST_(Input: "\"Hello\"", StmtIdx: 0, Expected: "\"Hello\";")
	TEST_(Input: "\"Hello\"\"World\"", StmtIdx: 1, Expected: "\"World\";")
	TEST_(Input: "1;2;", StmtIdx: 1, Expected: "2;")
	TEST_(Input: ";49;", StmtIdx: 0, Expected: "49;")
	TEST_(Input: ";!49;!3", StmtIdx: 0, Expected: "!(49);")
	TEST_(Input: ";!49;;;;;;\"Random String\";;;;;;;;;;!59;", StmtIdx: 2, Expected: "!(59);")
	TEST_(Input: ";!49;;;;;;\"Random String\";;;;;;;;;;!59;", StmtIdx: 0, Expected: "!(49);")
	TEST_(Input: "1+1", StmtIdx: 0, Expected: "(1+1);")
	TEST_(Input: "1*1", StmtIdx: 0, Expected: "(1*1);")
	TEST_(Input: "1/1", StmtIdx: 0, Expected: "(1/1);")
	TEST_(Input: "1-1", StmtIdx: 0, Expected: "(1-1);")
	TEST_(Input: "!1-1", StmtIdx: 0, Expected: "(!(1)-1);")
	TEST_(Input: "1*1+4", StmtIdx: 0, Expected: "((1*1)+4);")
	TEST_(Input: "1+1*4", StmtIdx: 0, Expected: "(1+(1*4));")
	TEST_(Input: "1*!1+4", StmtIdx: 0, Expected: "((1*!(1))+4);")
	TEST_(Input: "1/4*4", StmtIdx: 0, Expected: "((1/4)*4);")
	TEST_(Input: "1*92/4+1*1-5", StmtIdx: 0, Expected: "((((1*92)/4)+(1*1))-5);")
	TEST_(Input: "1*92/(4+1)*1-5", StmtIdx: 0, Expected: "((((1*92)/(4+1))*1)-5);")
	TEST_(Input: "1/(4*4)", StmtIdx: 0, Expected: "(1/(4*4));")
	TEST_(Input: "1*(4+4)", StmtIdx: 0, Expected: "(1*(4+4));")
	return 0;
}
