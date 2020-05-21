#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H
#include <stdio.h>
#include <stdbool.h>
/**/
extern const char* token_types[];
extern const char* keywords[];
/**/
typedef unsigned char TokenType;
#define JKT_KEYWORD 1
#define JKT_SYMBOL 2
#define JKT_IDENTIFIER 3
#define JKT_INT_CONST 4
#define JKT_STRING_CONST 5
/**/
typedef unsigned char Keyword;
#define JKT_CLASS 1
#define JKT_METHOD 2
#define JKT_FUNCTION 3
#define JKT_CONSTRUCTOR 4
#define JKT_INT 5
#define JKT_BOOLEAN 6
#define JKT_CHAR 7
#define JKT_VOID 8
#define JKT_VAR 9
#define JKT_STATIC 10
#define JKT_FIELD 11
#define JKT_LET 12
#define JKT_DO 13
#define JKT_IF 14
#define JKT_ELSE 15
#define JKT_WHILE 16
#define JKT_RETURN 17
#define JKT_TRUE 18
#define JKT_FALSE 19
#define JKT_NULL 20
#define JKT_THIS 21
/**/
typedef struct {
	FILE* src;//jack source file
	TokenType tok_t;//current token type
	Keyword keyword;//current keyword
	char *token;//current token
	int tok_size;//token size
} JackTokenizer;
/**/
void setJackTokenizer(JackTokenizer*, FILE*);
void unsetJackTokenizer(JackTokenizer*);
bool hasMoreTokens(JackTokenizer*);
void advance(JackTokenizer*);
TokenType getTokenType(JackTokenizer*);
Keyword getKeyword(JackTokenizer*);
char getSymbol(JackTokenizer*);
char* getIdentifier(JackTokenizer*);
int getIntVal(JackTokenizer*);
char* getStringVal(JackTokenizer*);

#endif
