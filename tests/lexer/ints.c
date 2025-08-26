#include "tokens.h"
#include "lexer.h"
#include "test.h"
#include <stdlib.h>

#define _TEST(numProvided, num, idx_lexer) if( testLexer(numProvided, num, idx_lexer, testnum++) != 0) { return 1; }

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
	int testnum = 0;
	_TEST("2910", "2910", 0);
	_TEST("4909", "4909", 0);
	_TEST("8713", "8713", 0);
	_TEST("8713 4909", "4909", 1);
	_TEST("4909 2910", "2910", 1);
	_TEST("4909 29102 97210", "97210", 2);
	_TEST("4909 2910 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 97210 17310", "17310", 32);
	return 0;
}
