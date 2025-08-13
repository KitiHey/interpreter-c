#include "lexer.h"
#include "tokens.h"
#include "test.h"
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static char consume(char* code, int *idx) {
		if (code[*idx] == '\0') {
				return code[*idx];
		}
		return code[ (*idx)++ ];
}
static bool isDigit(char* codeFile, int idx);


#define Peek() codeFile[idx]
#define PeekIs(peekChr) (Peek()==peekChr)
#define Consume() consume(codeFile, (int *) &idx)

#define SkipUnnecesary() if (PeekIs(' ')) {\
								Consume();\
								continue;} 

#define IncrementSpace() if (LexIdx+1 >= Allocated) { \
								Allocated *= 2;\
								token_t *newLex=realloc(Lex,Allocated*sizeof(token_t)); \
								assert(newLex!=NULL); \
								Lex=newLex; \
						} assert(Allocated>LexIdx);

#define buffEquals(strExpected, tokenType) if (strcmp(buffer, strExpected) == 0) { \
								Add() { .literal = strExpected, .type = tokenType}; \
								continue; \
							}

#define peekEquals(charExpected, tokenType, strLiteral) if (PeekIs((char) charExpected)) { \
								Add() { .type = tokenType, .literal = (char*) strLiteral }; \
								Consume(); \
								continue; \
							}
 
#define Add() IncrementSpace(); Lex[LexIdx++] = (token_t)
#define Isdigit() isDigit(codeFile, (int) idx)

token_t *Lexer(char* codeFile) {
		if (codeFile==NULL) return NULL;

		token_t *Lex = calloc(2, sizeof(token_t));
		assert(Lex!=NULL);
		size_t len = strlen(codeFile);

		int idx = 0;
		int LexIdx = 0;
		int Allocated = 2;

		char buffer[len-1];
		while (!PeekIs('\0')) {
				buffer[0] = '\0';

				SkipUnnecesary();
				peekEquals(';', SEMICOLON, ";");
				peekEquals('=', EQUAL, "=");

				peekEquals('+', PLUS, "+");
				peekEquals('-', MINUS, "-");
				peekEquals('*', ASTERISK, "*");
				peekEquals('/', SLASH, "/");

				peekEquals('(', L_PARENT, "(");
				peekEquals(')', R_PARENT, ")");
				peekEquals('{', L_BRACKET, "{");
				peekEquals('!', BANG, "!");
				peekEquals('}', R_BRACKET, "}");
				peekEquals('[', L_BRACE, "[");
				peekEquals(']', R_BRACE, "]");

				bool foundChar = false;
				int iBuffer = 0;

				if (PeekIs('"')) {
					Consume();
					while (!PeekIs('"') && !PeekIs('\0')) {
						assert(iBuffer<=len-1);
						buffer[iBuffer++] = Consume();
					}
					if (PeekIs('\0')) {
						free(Lex);
						Lex=NULL;
						return NULL;
					}
					buffer[iBuffer] = '\0';
					Add(){ 
							.literal = strdup(buffer),
							.type = STRING,
					};
					Consume();
					continue;
				}
				
#define P(arg) && !PeekIs(arg)
				while (!PeekIs(';') P('=') P('!') P(' ') P('\0') P('+') P('-') P('*') P('/') P('(') P(')') P('{') P('}') P(']') P('[') ) {
#undef P
						test(bool oldchar = foundChar);
						foundChar = foundChar ? true: !Isdigit();

						test(bool expectedResult = Isdigit() ? foundChar==oldchar: foundChar==true);
						testAssert(expectedResult);

						assert(iBuffer<=len-1);
						buffer[iBuffer++] = Consume();
				}
				buffer[iBuffer] = '\0';

				if (!foundChar) {
						Add(){
							.literal = buffer,
							.type = INT,
						};
						continue;
				}
				buffEquals("let", LET);
				buffEquals("func", FUNC);
				buffEquals("return", RETURN);
				buffEquals("if", IF);
				buffEquals("else", ELSE);
				Add(){ 
						.literal = strdup(buffer),
						.type = IDENT,
				};
		}
		Lex[LexIdx] = (token_t) {
				.literal = "EOF",
				.type = TERMINATE,
		};
		return Lex;
};

static bool isDigit(char* codeFile, int idx) {
		char character = Peek();
		if (character>='0' && character<='9') return true;
		return false;
}

#undef Add
#undef Isdigit
#undef peekEquals
#undef buffEquals
#undef IncrementSpace
#undef SkipUnnecesary
#undef Consume
#undef Peek
#undef PeekIs
