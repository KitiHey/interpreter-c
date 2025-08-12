#pragma once

typedef enum TokenType {
	NONE,
	TERMINATE,

	INT,
	STRING,
	IDENT,

	PLUS,
	MINUS,
	ASTERISK,
	SLASH,

	L_BRACKET, // {
	R_BRACKET, // }
	L_PARENT, // (
	R_PARENT, // )
	L_BRACE, // [
	R_BRACE, // ]

	EQUAL,
	SEMICOLON, // Optional

	IF,
	ELSE,
	FUNC,
	
	RETURN,
	LET,
} tokenType_t;

typedef struct Token {
	char* literal;
	tokenType_t type;
} token_t;
