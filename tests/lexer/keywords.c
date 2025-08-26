#include "test.h"
#include "lexer.h"
#include "tokens.h"
#include <stdlib.h>

static void testFunc(char* str, int idx, char* expected, tokenType_t expectedType, int* testNum) {
		lexer_t* LexerT = Lexer(str);
		token_t *L = LexerT->tokens;
		nequal(L, NULL) {
				error(*testNum, "Returned NULL '%s'", str);
				(*testNum)++;
				return;
		}
		equal(L[idx].literal, expected) {
				error(*testNum, "Not the expected literal: '%s'!='%s'", L[idx].literal, expected);
				goto terminate;
		}
		equal(L[idx].type, expectedType) {
				error(*testNum, "Not the expected type: '%d'!='%d'", L[idx].type, expectedType);
				goto terminate;
		}
		success(*testNum, "Returned expected '%s'", expected);
terminate:
		free(L);
		L=NULL;
		(*testNum)++;
}

int main() {
		int testNum = 0;
		testFunc("func return=let", 0, "func", FUNC, &testNum);
		testFunc("func return=let", 1, "return", RETURN, &testNum);
		testFunc("func return=let", 2, "=", EQUAL, &testNum);
		testFunc("func return(=let", 4, "let", LET, &testNum);
		testFunc("func }return=let", 5, "EOF", TERMINATE, &testNum);
		testFunc(";;yo", 1, ";", SEMICOLON, &testNum);
		testFunc(";;yo", 2, "yo", IDENT, &testNum);
		testFunc(";;yo!", 2, "yo", IDENT, &testNum);
		testFunc(";;yo;!{", 2, "yo", IDENT, &testNum);
		testFunc(";;\"yo o\";!{", 2, "yo o", STRING, &testNum);
		testFunc(";;yo!", 3, "!", BANG, &testNum);
		testFunc("\"Hello World!\"", 0, "Hello World!", STRING, &testNum);
		testFunc("\"Hello\" \"World!\"", 1, "World!", STRING, &testNum);
		testFunc("\"Hello\";;;\"World!\"", 4, "World!", STRING, &testNum);
		testFunc(";=else", 2, "else", ELSE, &testNum);
		testFunc(";=if", 2, "if", IF, &testNum);
		testFunc(";=if,", 3, ",", COMMA, &testNum);
		testFunc(";=if,!=", 4, "!=", BOOL_NOT_EQUALS, &testNum);
		testFunc(";=if,==", 4, "==", BOOL_EQUALS, &testNum);
		testFunc(";=if,===!=", 4, "==", BOOL_EQUALS, &testNum);
		testFunc(";=if,===!=", 5, "=", EQUAL, &testNum);
		return 0;
}
