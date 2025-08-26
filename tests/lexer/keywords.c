#include "test.h"
#include "lexer.h"
#include "tokens.h"
#include <stdlib.h>

#define _TEST(str, idx, expected, expectedType) if (testFunc(str, idx, expected, expectedType, &testNum) != 0) return 1;

static int testFunc(char* str, int idx, char* expected, tokenType_t expectedType, int* testNum) {
		lexer_t* LexerT = Lexer(str);
		token_t *L = LexerT->tokens;
		nequal(L, NULL) {
				error(*testNum, "Returned NULL '%s'", str);
				(*testNum)++;
				FREE_LEXER(LexerT);
				return 1;
		}
		equal(L[idx].literal, expected) {
				error(*testNum, "Not the expected literal: '%s'!='%s'", L[idx].literal, expected);
				FREE_LEXER(LexerT);
				return 1;
		}
		equal(L[idx].type, expectedType) {
				error(*testNum, "Not the expected type: '%d'!='%d'", L[idx].type, expectedType);
				FREE_LEXER(LexerT);
				return 1;
		}
		success(*testNum, "Returned expected '%s'", expected);
terminate:
		L=NULL;
		(*testNum)++;
		FREE_LEXER(LexerT);
		return 0;
}

int main() {
		int testNum = 0;
		_TEST("func return=let", 0, "func", FUNC);
		_TEST("func return=let", 1, "return", RETURN);
		_TEST("func return=let", 2, "=", EQUAL);
		_TEST("func return(=let", 4, "let", LET);
		_TEST("func }return=let", 5, "EOF", TERMINATE);
		_TEST(";;yo", 1, ";", SEMICOLON);
		_TEST(";;yo", 2, "yo", IDENT);
		_TEST(";;yo!", 2, "yo", IDENT);
		_TEST(";;yo;!{", 2, "yo", IDENT);
		_TEST(";;\"yo o\";!{", 2, "yo o", STRING);
		_TEST(";;yo!", 3, "!", BANG);
		_TEST("\"Hello World!\"", 0, "Hello World!", STRING);
		_TEST("\"Hello\" \"World!\"", 1, "World!", STRING);
		_TEST("\"Hello\";;;\"World!\"", 4, "World!", STRING);
		_TEST(";=else", 2, "else", ELSE);
		_TEST(";=if", 2, "if", IF);
		_TEST(";=if,", 3, ",", COMMA);
		_TEST(";=if,!=", 4, "!=", BOOL_NOT_EQUALS);
		_TEST(";=if,==", 4, "==", BOOL_EQUALS);
		_TEST(";=if,===!=", 4, "==", BOOL_EQUALS);
		_TEST(";=if,===!=", 5, "=", EQUAL);
		return 0;
}
