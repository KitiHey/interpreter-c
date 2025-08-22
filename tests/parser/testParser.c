#include "test.h"
#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include <stdlib.h>

#define TEST_(...) Test( (tests_t) { Idx: &Idx, __VA_ARGS__ } );

typedef struct Tests {
	char* Expected;
	char* Input;
	int* Idx;
} tests_t;

void Test(tests_t testT) {
		token_t *L = Lexer(testT.Input);
		program_t P = Parser(L);

		stmts_t *Stmt = P.Statements;
		nequal(Stmt, NULL) {
				error(*(testT.Idx), "stmts are NULL");
				return;
		}
		nequal(Stmt->Statements, NULL) {
				error(*(testT.Idx), "stmts are NULL");
				return;
		}
		equal(P.testString, testT.Expected) {
				error(*(testT.Idx), "got '%s' instead of '%s'", P.testString, testT.Expected);
				return;
		}
		success(*(testT.Idx), "got %s", testT.Expected);
		*(testT.Idx)++;
		free(P.Arena);
}

int main() {
	int Idx = 0;
// Strings
	TEST_(Input: "\"Hello\"", Expected: "\"Hello\";")
	TEST_(Input: "\"Hello\"\"World\"", Expected: "\"Hello\";\"World\";")
// Ints
	TEST_(Input: "1;2;", Expected: "1;2;")
	TEST_(Input: ";49;", Expected: "49;")
// Prefixes
	TEST_(Input: ";!49;!3", Expected: "!(49);!(3);")
	TEST_(Input: ";!49;;;;;;\"Random String\";;;;;;;;;;!59;", Expected: "!(49);\"Random String\";!(59);")
// Infixes
	TEST_(Input: "1+1", Expected: "(1+1);")
	TEST_(Input: "1*1", Expected: "(1*1);")
	TEST_(Input: "1/1", Expected: "(1/1);")
	TEST_(Input: "1-1", Expected: "(1-1);")
// Infixes Priorities
	TEST_(Input: "!1-1", Expected: "(!(1)-1);")
	TEST_(Input: "1*1+4", Expected: "((1*1)+4);")
	TEST_(Input: "1+1*4", Expected: "(1+(1*4));")
	TEST_(Input: "1*!1+4", Expected: "((1*!(1))+4);")
	TEST_(Input: "1/4*4", Expected: "((1/4)*4);")
	TEST_(Input: "1*92/4+1*1-5", Expected: "((((1*92)/4)+(1*1))-5);")
// Parenthesis
	TEST_(Input: "1*92/(4+1)*1-5", Expected: "((((1*92)/(4+1))*1)-5);")
	TEST_(Input: "1/(4*4)", Expected: "(1/(4*4));")
	TEST_(Input: "1*(4+4)", Expected: "(1*(4+4));")
// Idents
	TEST_(Input: "a*(b+c)", Expected: "(a*(b+c));")
	TEST_(Input: "a+\"Hello\"*\"World\"", Expected: "(a+(\"Hello\"*\"World\"));")
	TEST_(Input: "!a;", Expected: "!(a);")
// Ifs
	TEST_(Input: "if (a+3) { 1+3 };", Expected: "if ((a+3)) { (1+3); };")
	TEST_(Input: "if (a+3) { if (a+3) { 1+3 }; };", Expected: "if ((a+3)) { if ((a+3)) { (1+3); }; };")
	TEST_(Input: "if (a+3) { 1+3 } else { \"Hello World!\" };", Expected: "if ((a+3)) { (1+3); } else { \"Hello World!\"; };")
// Funcs
	TEST_(Input: "func (a, b, c) { a+b };", Expected: "func (a, b, c) { (a+b); };")
	TEST_(Input: "func () { a+b };", Expected: "func () { (a+b); };")
	TEST_(Input: "func () { 3+1 };", Expected: "func () { (3+1); };")
	TEST_(Input: "func (a,b) { 3 };", Expected: "func (a, b) { 3; };")
	return 0;
}
