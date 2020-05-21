#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H 
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <stdio.h>
typedef struct {
	JackTokenizer *src;//JackTokenizer Input
	VMWriter vmw;//VMWriter output
	FILE *tar;//xml target file
	SymbolTable symtab[2];
	char op_stack[256];//store operator
	int stack_top;
	Keyword func_t;//function type
	unsigned short if_label_c;//if label count
	unsigned short while_label_c;//while label count
} CompilationEngine;

void setCompilationEngine(CompilationEngine*, JackTokenizer*, FILE*);
void unsetCompilationEngine(CompilationEngine*);
void compileClass(CompilationEngine*);
void compileClassVarDec(CompilationEngine*);
void compileSubroutineDec(CompilationEngine*);
void compileParameterList(CompilationEngine*);
void compileSubroutineBody(CompilationEngine*);
void compileVarDec(CompilationEngine*);
void compileStatements(CompilationEngine*);
void compileLet(CompilationEngine*);
void compileIf(CompilationEngine*);
void compileWhile(CompilationEngine*);
void compileDo(CompilationEngine*);
void compileReturn(CompilationEngine*);
void compileExpression(CompilationEngine*);
void compileTerm(CompilationEngine*);
int compileExpressionList(CompilationEngine*);

#endif
