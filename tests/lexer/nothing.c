#include "tokens.h"
#include "lexer.h"
#include "test.h"

int main() {
	token_t *L = Lexer("");
	nequal(L, NULL) {
			error(0, "Returned NULL");
			return 1;
	}
	equal(L[0].type, TERMINATE){
		error(0, "nothing in Lexer");
		return 1;
	}
	success(0, "nothing in Lexer");
	return 0;
}
