#include "ast.h"
#include "parser.h"
#include "tokens.h"
#include "arena.h"
#include "pMacros.h"
#include <string.h>
#include <stdlib.h>

stmts_t *ParseStatements(token_t** Lexer, arena_t* Arena, tokenType_t delimiter) {
		SKIPSEMI();
		if (PEEK() == TERMINATE) return NULL;

		stmts_t *Stmt = MALLOC(sizeof(stmts_t));
		if (Stmt == NULL) return NULL;
#ifdef ALLOW_TESTS
		char* TestString = calloc(10,sizeof(char));
		if (TestString == NULL) return NULL;
		int capString = 10;
		int idxString = 0;
#endif
		statements_t** Statements = calloc(4, sizeof(expressions_t*));
		if (Statements == NULL) return NULL;
		int cap = 4;
		int idx = 0;
		for (idx = 0; PEEK() != TERMINATE && PEEK() != delimiter; idx++) {
				if (idx >= cap) {
						cap *= 2;
						Statements = realloc(Statements, cap*sizeof(statements_t*));
						if (Statements == NULL) return NULL;
				}
				Statements[idx] = ParseStmt(Lexer, Arena);
				if (Statements[idx] == NULL) return NULL;
#ifdef ALLOW_TESTS
				idxString += strlen(Statements[idx]->testString)+1;
				if (idxString >= capString) {
						while (idxString >= capString) {
								capString *= 2;
						}
						TestString = realloc(TestString, capString * sizeof(char));
						if (TestString == NULL) return NULL;
				}
				strcat(TestString, Statements[idx]->testString);
#endif
		}
		Stmt->Statements = MALLOC((idx+1)*sizeof(statements_t*));
		if (Stmt->Statements == NULL) return NULL;
#ifdef ALLOW_TESTS
		Stmt->testString = MALLOC((idxString+1) * sizeof(char));
		if (Stmt->testString == NULL) return NULL;
#endif

		memcpy(Stmt->Statements, Statements, idx+1);
		free(Statements);
		Statements=NULL;

#ifdef ALLOW_TESTS
		memcpy(Stmt->testString, TestString, idxString+1);
		free(TestString);
		TestString=NULL;
#endif
		return Stmt;
}

program_t Parser(token_t* Lexer) {
		program_t Program = {NULL};
		if (Lexer == NULL || Lexer[0].type == TERMINATE) return Program;

		Program.Arena = ArenaCreate(ARENA_CAP);
		if (Program.Arena == NULL) return Program;

		Program.Statements = ParseStatements(&Lexer, Program.Arena, TERMINATE);
		if (Program.Statements == NULL) {
				FREE(Program.Arena); 
				Program.Arena = NULL;
				return Program;
		}
#ifdef ALLOW_TESTS
		Program.testString = Program.Statements->testString;
#endif
		return Program;
}
