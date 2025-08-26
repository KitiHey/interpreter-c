#include "tokens.h"
#include "lexer.h"
#include "test.h"

int main() {
	lexer_t* LexerT = Lexer("");
	token_t *L = LexerT->tokens;
	nequal(L, NULL) {
			error(0, "Returned NULL");
			return 1;
	}
	equal(L[0].type, TERMINATE){
		error(0, "nothing in Lexer");
		return 1;
	}
	success(0, "nothing in Lexer");
	L = NULL;
	FREE_LEXER(LexerT);
	return 0;
}
