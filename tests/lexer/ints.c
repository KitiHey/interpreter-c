#include "tokens.h"
#include "lexer.h"
#include "test.h"
#include <stdlib.h>

int testLexer(char* numProvided, char* num, int idx_Lexer, int testNum) {
	lexer_t* LexerT = Lexer(numProvided);
	token_t *L = LexerT->tokens;
	nequal(L, NULL) {
			error(testNum, "Returned NULL");
			return 1;
	}
	equal(L[idx_Lexer].type, INT) {
			error(testNum, "Not an INT type: %d", L[idx_Lexer].type);
			return 1;
	}
	equal(L[idx_Lexer].literal, num) {
			error(testNum, "Not the expected int: %s!=%s", L[idx_Lexer].literal, num);
			return 1;
	}
	success(testNum, "Int lexed into %s", num);
	L=NULL;
	FREE_LEXER(LexerT);
	return 0;
}

int main() {
	testLexer("2910", "2910", 0, 1);
	testLexer("4909", "4909", 0, 0);
	testLexer("8713", "8713", 0, 2);
	testLexer("8713 4909", "4909", 1, 3);
	testLexer("4909 2910", "2910", 1, 4);
	testLexer("4909 29102 97210", "97210", 2, 5);
	testLexer("4909 2910 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 17310", "17310", 32, 6);
	return 0;
}
