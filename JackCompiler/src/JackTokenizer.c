#include "..\include\JackTokenizer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define FREE(p) do{ free(p); p = NULL; }while(0)
#define DEBUG() \
	fprintf(stderr, "%s: %s(Line %d) ", __FILE__, __func__, __LINE__)

const char *token_types[] = {
	"", "keyword", "symbol", "identifier",
	"integerConstant", "stringConstant"
};
const char *keywords[] = {
	"", "class", "method", "function", "constructor",
	"int", "boolean", "char", "void", "var",
	"static", "field", "let", "do", "if", "else",
	"while", "return", "true", "false", "null", "this"
};

void setJackTokenizer(JackTokenizer *jkt, FILE *src){
	jkt->src = src;
	jkt->tok_t = 0;
	jkt->keyword = 0;
	jkt->tok_size = BUFSIZ;
	jkt->token = malloc(BUFSIZ);//error: malloc(sizeof(BUFSIZ)) -> 4
	if(jkt->token == NULL){
		DEBUG();
		perror("malloc");
		exit(1);
	}
}
void unsetJackTokenizer(JackTokenizer *jkt){
	jkt->src = NULL;
	FREE(jkt->token);
}

bool hasMoreTokens(JackTokenizer *jkt){
	int ch;
	while((ch = fgetc(jkt->src)) != EOF){
		if(isalnum(ch)){
			if(ungetc(ch, jkt->src) != EOF){
				return true;
			}
			else{
				DEBUG();
				perror("ungetc");
				exit(1);
			}
		}
		else if(strchr("{}()[].,;+-*/&|<>=~\"_", ch) != NULL){
			if(ch == '/'){
				int ch2 = fgetc(jkt->src);
				if(ch2 == '/'){
					do {
						ch = fgetc(jkt->src);
						if(ch == EOF){
							return false;
						}
					} while(ch != '\n');
				}
				else if(ch2 == '*'){
					do{
						ch = fgetc(jkt->src);
						if(ch == '*'){
							ch2 = fgetc(jkt->src);
							if(ch2 == '/'){
								break;
							}
						}
					}while(ch != EOF);
				}
				else{
					if(ungetc(ch2, jkt->src) != EOF &&
						ungetc(ch, jkt->src) != EOF){
						return true;
					}
					else{
						DEBUG();
						perror("ungetc");
						exit(1);
					}
				}
			}
			else{
				if(ungetc(ch, jkt->src) != EOF){
					return true;
				}
				else{
					DEBUG();
					perror("ungetc");
					exit(1);
				}
			}
		}
	}
	return false;
}
void advance(JackTokenizer *jkt){
	int ch = fgetc(jkt->src);
	if(isdigit(ch)){
		int idx = 0;
		do{
			jkt->token[idx] = ch;
			idx++;
			ch = fgetc(jkt->src);
		}while(isdigit(ch));
		if(ch!=EOF){
			ungetc(ch, jkt->src);
		}
		jkt->token[idx] = '\0';
		jkt->tok_t = JKT_INT_CONST;
		jkt->keyword = 0;
	}
	else if(isalpha(ch) || ch == '_'){
		int idx = 0;
		do{
			jkt->token[idx] = ch;
			idx++;
			ch = fgetc(jkt->src);
		}while(isdigit(ch) || isalpha(ch) || ch == '_');
		if(ch!=EOF){
			ungetc(ch, jkt->src);
		}
		jkt->token[idx] = '\0';
		jkt->tok_t = JKT_IDENTIFIER;
		jkt->keyword = 0;
		int len = sizeof(keywords)/sizeof(char*);
		for(int i=1; i<len; i++){
			if(strcmp(jkt->token, keywords[i]) == 0){
				jkt->tok_t = JKT_KEYWORD;
				jkt->keyword = i;
				break;
			}
		}
	}
	else if(ch == '\"'){
		int idx = 0;
		while((ch = fgetc(jkt->src)) != '\"' && ch != EOF){
			jkt->token[idx] = ch;
			idx++;
			if(idx > jkt->tok_size-1){
				jkt->tok_size*=2;
				jkt->token = realloc(jkt->token, jkt->tok_size);
				if(jkt->token == NULL){
					DEBUG();
					perror("realloc");
					exit(1);
				}	
			}
		}
		jkt->token[idx] = '\0';
		jkt->tok_t = JKT_STRING_CONST;
		jkt->keyword = 0;
	}
	else if(strchr("{}()[].,;+-*/&|<>=~", ch) != NULL){
		jkt->token[0] = ch;
		jkt->token[1] = '\0';
		jkt->tok_t = JKT_SYMBOL;
		jkt->keyword = 0;
	}
	else{
		jkt->token[0] = '\0';
		jkt->tok_t = 0;
		jkt->keyword = 0;
	}
}

TokenType getTokenType(JackTokenizer *jkt){
	return jkt->tok_t;
}
Keyword getKeyword(JackTokenizer *jkt){
	if(jkt->tok_t == JKT_KEYWORD){
		return jkt->keyword;
	}
	return 0;
}
char getSymbol(JackTokenizer *jkt){
	if(jkt->tok_t == JKT_SYMBOL){
		return jkt->token[0];
	}
	return 0;
}
char* getIdentifier(JackTokenizer *jkt){
	if(jkt->tok_t == JKT_IDENTIFIER){
		return jkt->token;
	}
	return NULL;
}
int getIntVal(JackTokenizer *jkt){
	if(jkt->tok_t == JKT_INT_CONST){
		return atoi(jkt->token);
	}

	return 0;
}
char* getStringVal(JackTokenizer *jkt){
	if(jkt->tok_t == JKT_STRING_CONST){
		return jkt->token;
	}
	return NULL;
}