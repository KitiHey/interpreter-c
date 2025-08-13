#include "test.h"
#include "lexer.h"
#include "parser.h"

int main() {
	token_t *L = Lexer("");
	program_t P = Parser(L);
	equal(P.Statements, NULL) {
		error(0, "'Statements' isn't NULL");
		return 1;
	}
	success(0, "Nothing is returned!");
	return 0;
}
